#pragma once
#include "util/Exception.h"
#include <lmpl/windows_error.h>
#include <vector>

namespace lmpl
{
template <typename ComInterface> class ComPtr
{
  protected:
    ComInterface *pInterface;

  public:
    ComPtr() : pInterface{nullptr} {}

    explicit ComPtr(ComInterface *pInterface_in) : pInterface{pInterface_in} {}

    ComPtr(ComPtr &other)
    {
        this->pInterface = other.pInterface;
        pInterface->AddRef();
    }

    ComPtr(ComPtr &&other)
    {
        this->pInterface = other.pInterface;
        other.pInterface = nullptr;
    }

    ComPtr &operator=(ComPtr &other)
    {
        this->ComPtr::~ComPtr();
        this->pInterface = other.pInterface;
        pInterface->AddRef();
        return *this;
    }

    ComPtr &operator=(ComInterface *ptr)
    {
        this->ComPtr::~ComPtr();
        this->pInterface = ptr;
        return *this;
    }

    template <typename OtherInterface> ComPtr<OtherInterface> as()
    {
        OtherInterface *output;
        auto hres = pInterface->QueryInterface(&output);
        static const auto myTypename = typeid(ComInterface).name();
        WinThrowIfFailed(hres);
        return ComPtr<OtherInterface>(output);
    }

    operator ComInterface *() { return pInterface; }

    ComInterface *operator->() { return pInterface; }

    ComInterface **operator&() { return &pInterface; }

    ~ComPtr()
    {
        if (pInterface != nullptr)
        {
            pInterface->Release();
            pInterface = nullptr;
        }
    }
};

template <typename ComInterface>
ComPtr<ComInterface> ComPtrFrom(ComInterface *ptr)
{
    return ComPtr<ComInterface>(ptr);
}

template <typename ComInterface>
using ComPtrs = std::vector<ComPtr<ComInterface>>;

/*template <typename ComInterface>
class ComPtrs : public std::vector<ComInterface*>
{
public:
        ComPtrs()
                : std::vector<ComInterface*>() {}

        ComPtrs(ComPtrs&& other)
                : std::vector<ComInterface*>(std::move(other))
        {
                for (auto pInterface : *this)
                {
                        pInterface->AddRef();
                }
        }

        ~ComPtrs()
        {
                for (auto pInterface : *this)
                {
                        pInterface->Release();
                }
        }
};*/
} // namespace lmpl