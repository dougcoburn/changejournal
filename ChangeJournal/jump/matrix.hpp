#ifndef JUMP_MATRIX
#define JUMP_MATRIX

#include <vector>

namespace jump {
    template <typename T, typename A=std::allocator<T> >
    class matrix {
        typedef T value_type;
        typedef std::vector<value_type, A> container;
    public:
        class row {
        public:
            row(int a, matrix& parent) : column(a), parent(parent) {}

            value_type& operator[](int b) {
                if (b > parent.rows()) {
                    throw new std::out_of_range("The value of the second dimension is out of range.");
                }
                return parent.data[(column * parent.dim_b) + b];
            }

            value_type operator[](int b) const {
                if (b > parent.rows()) {
                    throw new std::out_of_range("The value of the second dimension is out of range.");
                }
                return parent.data[(column * parent.dim_b) + b];
            }
        private:
            int column;
            matrix& parent;
        };

        matrix() : data(), dim_b(0) {}
        matrix(int a, int b, value_type const& initial = value_type())
            : dim_b(0) {
            resize(a, b, initial);
        }

        void resize(int a, int b, value_type const& initial = value_type()) {
            if (0 == a) {
                b = 0;
            }
            data.resize(a*b, initial);
            dim_b = b;
        }
        row operator[](int a) {
            return row(a, *this);
        }
        const row operator[](int a) const {
            return row(a, const_cast<matrix&>(*this));
        }
        int columns() const {
            return dim_b ? data.size() / dim_b : 0;
        }
        int rows() const {
            return dim_b;
        }
    private:
        container data;
        int dim_b;
    };
}

#endif // JUMP_MATRIX