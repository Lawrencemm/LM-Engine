#pragma once
#include <tbb/flow_graph.h>

#include "tuple_index.h"
#include "worker_thread.h"

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

template <typename outputs_type, typename put_type, typename output_ports_type>
bool try_put(output_ports_type &output_ports, put_type &&put)
{
    return std::get<tuple_index<put_type, outputs_type>::value>(output_ports)
      .try_put(put);
}
} // namespace lm
