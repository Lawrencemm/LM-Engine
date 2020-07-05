#pragma once
#include "tuple_index.h"
#include "worker_thread.h"
#include <optional>
#include <tbb/flow_graph.h>

typedef tbb::flow::continue_node<tbb::flow::continue_msg> dependency_node;

namespace lm
{

inline dependency_node &operator>(dependency_node &lhs, dependency_node &rhs)
{
    tbb::flow::make_edge(lhs, rhs);
    return rhs;
}

template <typename input_type, typename output_type>
class wait_node : public tbb::flow::async_node<input_type, output_type>
{
    using super_type = tbb::flow::async_node<input_type, output_type>;

  public:
    template <typename body_type>
    wait_node(tbb::flow::graph &graph, body_type body)
        : super_type{
            graph,
            1,
            [this, body](
              input_type input,
              typename super_type::gateway_type &gateway) {
                gateway.reserve_wait();
                worker_thread.job([body, input, &gateway] {
                    gateway.try_put(body(input));
                    gateway.release_wait();
                });
            },
          }
    {
    }

  private:
    lm::worker_thread worker_thread;
};

template <
  typename sender_input_type,
  typename sender_outputs_type,
  typename receiver_input_type>
void make_edge(
  tbb::flow::multifunction_node<sender_input_type, sender_outputs_type> &sender,
  tbb::flow::receiver<receiver_input_type> &receiver)
{
    tbb::flow::make_edge(
      tbb::flow::output_port<
        tuple_index<receiver_input_type, sender_outputs_type>::value>(sender),
      receiver);
}

template <
  typename sender_input_type,
  typename sender_outputs_type,
  typename receiver_input_type>
void remove_edge(
  tbb::flow::multifunction_node<sender_input_type, sender_outputs_type> &sender,
  tbb::flow::receiver<receiver_input_type> &receiver)
{
    tbb::flow::remove_edge(
      tbb::flow::output_port<
        tuple_index<receiver_input_type, sender_outputs_type>::value>(sender),
      receiver);
}

template <typename sender_output_type, typename receiver_inputs_type>
void make_edge(
  tbb::flow::sender<sender_output_type> &sender,
  tbb::flow::join_node<receiver_inputs_type> &receiver)
{
    tbb::flow::make_edge(
      sender,
      tbb::flow::input_port<
        tuple_index<sender_output_type, receiver_inputs_type>::value>(
        receiver));
}

template <typename sender_output_type, typename receiver_inputs_type>
void remove_edge(
  tbb::flow::sender<sender_output_type> &sender,
  tbb::flow::join_node<receiver_inputs_type> &receiver)
{
    tbb::flow::remove_edge(
      sender,
      tbb::flow::input_port<
        tuple_index<sender_output_type, receiver_inputs_type>::value>(
        receiver));
}

template <typename outputs_type, typename put_type, typename output_ports_type>
bool try_put(output_ports_type &output_ports, put_type &&put)
{
    return std::get<tuple_index<put_type, outputs_type>::value>(output_ports)
      .try_put(put);
}

template <typename T> class limiter_node;

template <typename T>
class decrementer : public tbb::flow::receiver<int>, tbb::internal::no_copy
{
  public:
    T *my_node;

    tbb::task *try_put_task(const int &value) __TBB_override
    {
        tbb::task *result = my_node->decrement_counter(value);
        if (!result)
            result = tbb::flow::internal::SUCCESSFULLY_ENQUEUED;
        return result;
    }

    tbb::flow::graph &graph_reference() const __TBB_override
    {
        return my_node->my_graph;
    }

    friend class limiter_node<T>;

    void reset_receiver(tbb::flow::reset_flags f) __TBB_override
    {
        tbb::internal::suppress_unused_warning(f);
    }

    // Since decrementer does not make use of possibly unconstructed owner
    // inside its constructor, my_node can be directly initialized with 'this'
    // pointer passed from the owner, hence making method 'set_owner' needless.
    decrementer() : my_node(NULL) {}
    void set_owner(T *node) { my_node = node; }
};

//! Forwards messages only if the threshold has not been reached
/** This node forwards items until its threshold is reached.
    It contains no buffering.  If the downstream node rejects, the
    message is dropped. */
template <typename T>
class limiter_node : public tbb::flow::graph_node,
                     public tbb::flow::receiver<T>,
                     public tbb::flow::sender<T>
{
  public:
    typedef T input_type;
    typedef T output_type;
    typedef typename tbb::flow::receiver<input_type>::predecessor_type
      predecessor_type;
    typedef
      typename tbb::flow::sender<output_type>::successor_type successor_type;

    size_t my_threshold;
    size_t my_count; // number of successful puts
    size_t my_tries; // number of active put attempts
    tbb::flow::interface11::internal::
      reservable_predecessor_cache<T, tbb::spin_mutex>
        my_predecessors;
    tbb::spin_mutex my_mutex;
    tbb::flow::interface11::internal::broadcast_cache<T> my_successors;
    __TBB_DEPRECATED_LIMITER_EXPR(int init_decrement_predecessors;)

    friend class decrementer<T>;

    // Let decrementer call decrement_counter()
    friend class decrementer<limiter_node<T>>;

    bool check_conditions()
    { // always called under lock
        return (
          my_count + my_tries < my_threshold && !my_predecessors.empty() &&
          !my_successors.empty());
    }

    // only returns a valid task pointer or NULL, never SUCCESSFULLY_ENQUEUED
    tbb::task *forward_task()
    {
        input_type v;
        tbb::task *rval = NULL;
        bool reserved = false;
        {
            tbb::spin_mutex::scoped_lock lock(my_mutex);
            if (check_conditions())
                ++my_tries;
            else
                return NULL;
        }

        // SUCCESS
        // if we can reserve and can put, we consume the reservation
        // we increment the count and decrement the tries
        if ((my_predecessors.try_reserve(v)) == true)
        {
            reserved = true;
            if ((rval = my_successors.try_put_task(v)) != NULL)
            {
                {
                    tbb::spin_mutex::scoped_lock lock(my_mutex);
                    ++my_count;
                    --my_tries;
                    my_predecessors.try_consume();
                    if (check_conditions())
                    {
                        if (tbb::flow::interface11::internal::is_graph_active(
                              this->my_graph))
                        {
                            tbb::task *rtask =
                              new (tbb::task::allocate_additional_child_of(
                                *(this->my_graph.root_task())))
                                tbb::flow::interface11::internal::
                                  forward_task_bypass<limiter_node<T>>(*this);
                            tbb::flow::interface11::internal::
                              spawn_in_graph_arena(graph_reference(), *rtask);
                        }
                    }
                }
                return rval;
            }
        }
        // FAILURE
        // if we can't reserve, we decrement the tries
        // if we can reserve but can't put, we decrement the tries and release
        // the reservation
        {
            tbb::spin_mutex::scoped_lock lock(my_mutex);
            --my_tries;
            if (reserved)
                my_predecessors.try_release();
            if (check_conditions())
            {
                if (tbb::flow::interface11::internal::is_graph_active(
                      this->my_graph))
                {
                    tbb::task *rtask =
                      new (tbb::task::allocate_additional_child_of(
                        *(this->my_graph.root_task())))
                        tbb::flow::interface11::internal::forward_task_bypass<
                          limiter_node<T>>(*this);
                    __TBB_ASSERT(!rval, "Have two tasks to handle");
                    return rtask;
                }
            }
            return rval;
        }
    }

    void forward()
    {
        __TBB_ASSERT(false, "Should never be called");
        return;
    }

    tbb::task *decrement_counter(long long delta)
    {
        {
            tbb::spin_mutex::scoped_lock lock(my_mutex);
            if (delta > 0 && size_t(delta) > my_count)
                my_count = 0;
            else
                my_count -= size_t(delta);
        }
        return forward_task();
    }

    void initialize()
    {
        my_predecessors.set_owner(this);
        my_successors.set_owner(this);
        decrement.set_owner(this);
        tbb::internal::fgt_node(
          CODEPTR(),
          tbb::internal::FLOW_LIMITER_NODE,
          &this->my_graph,
          static_cast<tbb::flow::receiver<input_type> *>(this),
          static_cast<tbb::flow::receiver<int> *>(&decrement),
          static_cast<tbb::flow::sender<output_type> *>(this));
    }

    //! The internal receiver< DecrementType > that decrements the count
    decrementer<limiter_node<T>> decrement;

    limiter_node(
      tbb::flow::graph &g,
      __TBB_DEPRECATED_LIMITER_ARG2(
        size_t threshold,
        int num_decrement_predecessors = 0))
        : graph_node(g),
          my_threshold(threshold),
          my_count(0),
          __TBB_DEPRECATED_LIMITER_ARG4(
            my_tries(0),
            decrement(),
            init_decrement_predecessors(num_decrement_predecessors),
            decrement(num_decrement_predecessors))
    {
        initialize();
    }

#if __TBB_PREVIEW_FLOW_GRAPH_NODE_SET
    template <typename... Args>
    limiter_node(const node_set<Args...> &nodes, size_t threshold)
        : limiter_node(nodes.graph_reference(), threshold)
    {
        make_edges_in_order(nodes, *this);
    }
#endif

    //! Copy constructor
    limiter_node(const limiter_node &src)
        : graph_node(src.my_graph),
          tbb::flow::receiver<T>(),
          tbb::flow::sender<T>(),
          my_threshold(src.my_threshold),
          my_count(0),
          __TBB_DEPRECATED_LIMITER_ARG4(
            my_tries(0),
            decrement(),
            init_decrement_predecessors(src.init_decrement_predecessors),
            decrement(src.init_decrement_predecessors))
    {
        initialize();
    }

    //! Replace the current successor with this new successor
    bool register_successor(successor_type &r) __TBB_override
    {
        tbb::spin_mutex::scoped_lock lock(my_mutex);
        bool was_empty = my_successors.empty();
        my_successors.register_successor(r);
        // spawn a forward task if this is the only successor
        if (
          was_empty && !my_predecessors.empty() &&
          my_count + my_tries < my_threshold)
        {
            if (tbb::flow::interface11::internal::is_graph_active(
                  this->my_graph))
            {
                tbb::task *task = new (tbb::task::allocate_additional_child_of(
                  *(this->my_graph.root_task())))
                  tbb::flow::interface11::internal::forward_task_bypass<
                    limiter_node<T>>(*this);
                tbb::flow::interface11::internal::spawn_in_graph_arena(
                  graph_reference(), *task);
            }
        }
        return true;
    }

    //! Removes a successor from this node
    /** r.remove_predecessor(*this) is also called. */
    bool remove_successor(successor_type &r) __TBB_override
    {
        r.remove_predecessor(*this);
        my_successors.remove_successor(r);
        return true;
    }

    //! Adds src to the list of cached predecessors.
    bool register_predecessor(predecessor_type &src) __TBB_override
    {
        tbb::spin_mutex::scoped_lock lock(my_mutex);
        my_predecessors.add(src);
        if (
          my_count + my_tries < my_threshold && !my_successors.empty() &&
          tbb::flow::interface11::internal::is_graph_active(this->my_graph))
        {
            tbb::task *task = new (tbb::task::allocate_additional_child_of(
              *(this->my_graph.root_task())))
              tbb::flow::interface11::internal::forward_task_bypass<
                limiter_node<T>>(*this);
            tbb::flow::interface11::internal::spawn_in_graph_arena(
              graph_reference(), *task);
        }
        return true;
    }

    //! Removes src from the list of cached predecessors.
    bool remove_predecessor(predecessor_type &src) __TBB_override
    {
        my_predecessors.remove(src);
        return true;
    }

    template <typename R, typename B> friend class run_and_put_task;
    template <typename X, typename Y>
    friend class tbb::flow::interface11::internal::broadcast_cache;
    template <typename X, typename Y>
    friend class tbb::flow::interface11::internal::round_robin_cache;
    //! Puts an item to this receiver
    tbb::task *try_put_task(const T &t) __TBB_override
    {
        {
            tbb::spin_mutex::scoped_lock lock(my_mutex);
            if (my_count + my_tries >= my_threshold)
                return NULL;
            else
                ++my_tries;
        }

        tbb::task *rtask = my_successors.try_put_task(t);

        if (!rtask)
        { // try_put_task failed.
            tbb::spin_mutex::scoped_lock lock(my_mutex);
            --my_tries;
            if (
              check_conditions() &&
              tbb::flow::interface11::internal::is_graph_active(this->my_graph))
            {
                rtask = new (tbb::task::allocate_additional_child_of(
                  *(this->my_graph.root_task())))
                  tbb::flow::interface11::internal::forward_task_bypass<
                    limiter_node<T>>(*this);
            }
        }
        else
        {
            tbb::spin_mutex::scoped_lock lock(my_mutex);
            ++my_count;
            --my_tries;
        }
        return rtask;
    }

    tbb::flow::graph &graph_reference() const __TBB_override
    {
        return my_graph;
    }

    void reset_receiver(tbb::flow::reset_flags /*f*/) __TBB_override
    {
        __TBB_ASSERT(false, NULL); // should never be called
    }

    void reset_node(tbb::flow::reset_flags f) __TBB_override
    {
        my_count = 0;
        if (f & tbb::flow::interface11::rf_clear_edges)
        {
            my_predecessors.clear();
            my_successors.clear();
        }
        else
        {
            my_predecessors.reset();
        }
        decrement.reset_receiver(f);
    }
}; // limiter_node

template <typename T>
class overwrite_node : public tbb::flow::graph_node,
                       public tbb::flow::receiver<T>,
                       public tbb::flow::sender<T>
{
  public:
    typedef T input_type;
    typedef T output_type;
    typedef typename tbb::flow::receiver<input_type>::predecessor_type
      predecessor_type;
    typedef
      typename tbb::flow::sender<output_type>::successor_type successor_type;

    __TBB_NOINLINE_SYM explicit overwrite_node(tbb::flow::graph &g)
        : graph_node(g)
    {
        my_successors.set_owner(this);
        tbb::internal::fgt_node(
          CODEPTR(),
          tbb::internal::FLOW_OVERWRITE_NODE,
          &this->my_graph,
          static_cast<tbb::flow::receiver<input_type> *>(this),
          static_cast<tbb::flow::sender<output_type> *>(this));
    }

    //! Copy constructor; doesn't take anything from src; default won't work
    __TBB_NOINLINE_SYM overwrite_node(const overwrite_node &src)
        : graph_node(src.my_graph),
          tbb::flow::receiver<T>(),
          tbb::flow::sender<T>(),
          reserved(false)
    {
        my_successors.set_owner(this);
        tbb::internal::fgt_node(
          CODEPTR(),
          tbb::internal::FLOW_OVERWRITE_NODE,
          &this->my_graph,
          static_cast<tbb::flow::receiver<input_type> *>(this),
          static_cast<tbb::flow::sender<output_type> *>(this));
    }

    ~overwrite_node() {}

    bool register_successor(successor_type &s) __TBB_override
    {
        using namespace tbb;

        spin_mutex::scoped_lock l(my_mutex);
        if (
          my_buffer &&
          tbb::flow::interface11::internal::is_graph_active(my_graph))
        {
            // We have a valid value that must be forwarded immediately.
            bool ret = s.try_put(my_buffer.value());
            if (ret)
            {
                // We add the successor that accepted our put
                my_successors.register_successor(s);
                my_buffer.reset();
            }
            else
            {
                // In case of reservation a race between the moment of
                // reservation and register_successor can appear, because failed
                // reserve does not mean that register_successor is not ready to
                // put a message immediately. We have some sort of infinite
                // loop: reserving node tries to set pull state for the edge,
                // but overwrite_node tries to return push state back. That is
                // why we have to break this loop with task creation.
                task *rtask = new (task::allocate_additional_child_of(
                  *(my_graph.root_task()))) register_predecessor_task(*this, s);
                tbb::flow::interface11::internal::spawn_in_graph_arena(
                  my_graph, *rtask);
            }
        }
        else
        {
            // No valid value yet, just add as successor
            my_successors.register_successor(s);
        }
        return true;
    }

    bool remove_successor(successor_type &s) __TBB_override
    {
        tbb::spin_mutex::scoped_lock l(my_mutex);
        my_successors.remove_successor(s);
        return true;
    }

    bool try_get(input_type &v) __TBB_override
    {
        tbb::spin_mutex::scoped_lock l(my_mutex);
        if (my_buffer && !reserved)
        {
            v = my_buffer.value();
            my_buffer.reset();
            return true;
        }
        return false;
    }

    //! Reserves an item
    bool try_reserve(T &v) __TBB_override
    {
        tbb::spin_mutex::scoped_lock l(my_mutex);
        if (my_buffer)
        {
            v = my_buffer.value();
            reserved = true;
            return true;
        }
        return false;
    }

    //! Releases the reserved item
    bool try_release() __TBB_override
    {
        tbb::spin_mutex::scoped_lock l(my_mutex);
        reserved = false;
        return true;
    }

    //! Consumes the reserved item
    bool try_consume() __TBB_override
    {
        tbb::spin_mutex::scoped_lock l(my_mutex);
        my_buffer.reset();
        reserved = false;
        return true;
    }

    bool is_valid()
    {
        tbb::spin_mutex::scoped_lock l(my_mutex);
        return my_buffer;
    }

  protected:
    template <typename R, typename B> friend class run_and_put_task;
    template <typename X, typename Y>
    friend class tbb::flow::interface11::internal::broadcast_cache;
    template <typename X, typename Y>
    friend class tbb::flow::interface11::internal::round_robin_cache;
    tbb::task *try_put_task(const input_type &v) __TBB_override
    {
        tbb::spin_mutex::scoped_lock l(my_mutex);
        return try_put_task_impl(v);
    }

    tbb::task *try_put_task_impl(const input_type &v)
    {
        my_buffer = v;
        tbb::task *rtask = my_successors.try_put_task(v);
        if (!rtask)
            rtask = tbb::flow::interface11::SUCCESSFULLY_ENQUEUED;
        return rtask;
    }

    tbb::flow::graph &graph_reference() const __TBB_override
    {
        return my_graph;
    }

    //! Breaks an infinite loop between the node reservation and
    //! register_successor call
    struct register_predecessor_task : public tbb::flow::interface11::graph_task
    {
        register_predecessor_task(predecessor_type &owner, successor_type &succ)
            : o(owner), s(succ){};

        tbb::task *execute() __TBB_override
        {
            if (!s.register_predecessor(o))
            {
                o.register_successor(s);
            }
            return NULL;
        }

        predecessor_type &o;
        successor_type &s;
    };

    tbb::spin_mutex my_mutex;
    tbb::flow::interface11::internal::
      broadcast_cache<input_type, tbb::null_rw_mutex>
        my_successors;

    std::optional<input_type> my_buffer;
    bool reserved;
    void reset_receiver(tbb::flow::reset_flags /*f*/) __TBB_override {}

    void reset_node(tbb::flow::reset_flags f) __TBB_override
    {
        my_buffer.reset();
        reserved = false;
        if (f & tbb::flow::rf_clear_edges)
        {
            my_successors.clear();
        }
    }
}; // overwrite_node
} // namespace lm
