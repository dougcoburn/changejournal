#include <Windows.h>

namespace jump {
    struct HandleTraits {
        static HANDLE invalid() throw() {
            return nullptr;
        }
        static void close(HANDLE value) throw() {
            CloseHandle(value);
        }
    };

    template<typename Type, typename Traits>
    class unique_handle {
    private:
        Type m_value;

        unique_handle(unique_handle const &);
        unique_handle& operator=(unique_handle const &);
        void close() throw() {
            if (*this) {
                Traits::close(m_value);
            }
        }
        struct boolean_struct { int member; };
        typedef int boolean_struct::* boolean_type;

        bool operator==(unique_handle const &);
        bool operator!=(unique_handle const &);

    public:
        explicit unique_handle(Type value = Traits::invalid()) throw()
            : m_value((value == INVALID_HANDLE_VALUE) ? Traits::invalid() : value) {}
        ~unique_handle() throw() {
            close();
        }

        operator boolean_type() const throw() {
            return Traits::invalid() != m_value ? &boolean_struct::member : nullptr;
        }

        Type get() const throw() {
            return m_value;
        }
    };
    typedef unique_handle<HANDLE, HandleTraits> Handle;
}