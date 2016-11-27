#include <iostream>
#include <gmp.h>

using namespace std;
// mpz_t magic(mpz_t x, mpz_t N)
// {
//     mpz_t rop, r;
//     mpz_init(rop);
//     mpz_init(r);
//     mpz_mul(rop, x, x);
//     mpz_add_ui(rop, 1);
//     mpz_mod(r, rop, N);
//     return r;
// }
int main()
{

    char arr[] = "30";

    gmp_randstate_t state;
    uint two = 2;
    mpz_t N,
        x, rop, a, b, p;
    unsigned int one = 1;
    mpz_init(rop);
    mpz_init(N);
    mpz_init(p);
    mpz_init(a);
    mpz_init(b);
    mpz_set_str(p, "1", 10);
    mpz_set_str(N, arr, 10);
    // mpz_init(x);
    // mpz_set_str(x, "4", 10);
    // mpz_init(rop);
    // mpz_gcd(rop, N, x);
    // gmp_printf("GCD is an mpz %Zd\n", rop);

    // could be used as the start point for the next test if first try fails.
    // initialization with 2
    mpz_set_str(a, "2", 10);
    mpz_set_str(b, "2", 10);
    mpz_set(rop, p);
    int i, j;
    cout << "Athayathu ramana" << mpz_cmp(rop, p) << std::endl;
    while (mpz_cmp(rop, p) == 0)
    {
        i++;
        // a = magic(a, N);
        // b = magic(magic(b, N), N);

        // f(x)
        mpz_mul(rop, a, a);
        mpz_add(rop, rop, p);
        mpz_mod(a, rop, N);

        // f(f(x))
        mpz_mul(rop, b, b);
        mpz_add(rop, rop, p);
        mpz_mod(b, rop, N);
        mpz_mul(rop, b, b);
        mpz_add(rop, rop, p);
        mpz_mod(b, rop, N);

        mpz_sub(rop, a, b);
        mpz_abs(rop, rop);
        mpz_gcd(rop, N, rop);
        gmp_printf("GCD is an mpz %Zd\n", rop);
        if (mpz_cmp(rop, p) > 0)
        {
            // factor found
            if (mpz_cmp(rop, N) == 0)
            {
                cout << "Here j" << j++ << endl;
                gmp_randinit_mt(state);
                mpz_sub_ui(rop, N, two); //N-2
                mpz_urandomm(rop, state, rop);
                mpz_add_ui(rop, rop, two);

                mpz_set(a, rop);
                mpz_set(b, a);
                mpz_set(rop, p);
            }
            else
            {
                break;
                gmp_printf("Factor is %Zd\n", rop);
            }
        }

        if (i > 100000)
        {
            break;
        }
    }

    cout << "Failed";
    return 0;
}