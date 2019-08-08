#pragma once
template <class F>
class finally
{
public:
    explicit finally(F f) noexcept 
      : f_(std::move(f)), invoke_(true) {}
	    finally(finally&& other) noexcept 
     : f_(std::move(other.f_)), 
       invoke_(other.invoke_)
    {
        other.invoke_ = false;
    }

	finally(const finally&) = delete;

    finally& operator=(const finally&) = delete;

    ~finally() noexcept
    {
        if (invoke_) f_();
    }

private:
    F f_;
    bool invoke_;
};