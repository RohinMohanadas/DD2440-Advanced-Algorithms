#include <iostream>
#include <malloc.h>
#include <gmp.h>
#include <ctime>

using namespace std;

mpz_t *fact_arr;
int indx = 0;

bool pollard2(mpz_t ret, mpz_t Number)
{
    int counter = 0;
    mpz_t x, y, p, d, randLimit, rop;
    mpz_init(x);
    mpz_init(y);
    mpz_init(p);
    mpz_init(d);
    mpz_init(rop);
    mpz_set_str(x, "2", 10); //  x is 2 initially

    mpz_mul(rop, x, x);
    mpz_add_ui(rop, rop, 1);   //  rop = f x
    mpz_mod(rop, rop, Number); //  hare
    mpz_set(y, rop);           // y is f(x)

    mpz_sub(rop, x, y);
    mpz_abs(rop, rop);
    mpz_gcd(p, rop, Number);

    while (mpz_cmp_ui(p, 1) == 0)
    {
        if (counter >= 100000)
        {
            // probable prime
            mpz_set_str(ret, "0", 10);
            return false;
        }
        mpz_mul(x, x, x);
        mpz_add_ui(x, x, 1);   //  rop = f x
        mpz_mod(x, x, Number); //  hare

        mpz_mul(y, y, y);
        mpz_add_ui(y, y, 1);   //  rop = f x
        mpz_mod(y, y, Number); //  hare
        mpz_mul(y, y, y);
        mpz_add_ui(y, y, 1);   //  rop = f x
        mpz_mod(y, y, Number); //  hare

        mpz_sub(rop, x, y);
        mpz_abs(rop, rop);
        mpz_gcd(p, rop, Number);
        counter++;
    }
    if (mpz_cmp(p, Number) == 0)
    {
        // cout << "Failure" << endl;
        mpz_set_str(ret, "0", 10);
        return false;
    }
    mpz_set(ret, p);
    return true;
}

void printer(mpz_t factor, bool print, bool flush)
{

    if (flush == true)
    {
        indx = 0;
        cout << "fail" << endl
             << endl;
    }
    else if (print == true)
    {
        mpz_init2(fact_arr[indx], 100);
        mpz_set(fact_arr[indx++], factor);
        for (int i = 0; i < indx; i++)
        {
            gmp_printf("%Zd\n", fact_arr[i]);
        }
        // cout << endl;
        indx = 0;
    }
    else
    {
        mpz_init2(fact_arr[indx], 100);
        mpz_set(fact_arr[indx++], factor);
    }
}
int main()
{
    int count = 1000, bits = 100, i = 0;
    mpz_t *num_arr;
    mpz_t *res_arr;
    mpz_t **sol_arr;
    mpz_t z, rop, two, limit;
    clock_t tStart = clock();
    clock_t tCurr;
    bool failFlag[2];
    num_arr = (mpz_t *)malloc(count * sizeof(mpz_t));
    fact_arr = (mpz_t *)malloc(100 * sizeof(mpz_t)); //max 100 factors
    res_arr = (mpz_t *)malloc(2 * sizeof(mpz_t));
    mpz_init2(res_arr[0], bits);
    mpz_init2(res_arr[1], bits);
    //  init
    mpz_init(z);
    mpz_init(rop);
    mpz_init(two);
    mpz_init(limit);
    mpz_set_str(two, "2", 10);
    mpz_set_str(limit, "175891579187581657617", 10);

    // mpz_array_init(*arr, count, bits);
    while (gmp_scanf("%Zd", z) == 1)
    {
        mpz_init2(num_arr[i], bits);
        mpz_set(num_arr[i++], z);
        // cout << "Scan :" << i << endl;
    }
    bool flag[i + 1] = {0};

    for (int k = 0; k < i; k++)
    {
        if (mpz_probab_prime_p(num_arr[k], 50) == 2)
        {
            gmp_printf("%Zd\n", num_arr[k]);
        }
        else
        {
            if (pollard2(rop, num_arr[k]))
            {
                naive(rop, k);
                mpz_cdiv_q(rop, num_arr[k], rop);
                naive(num_arr[k], k);
            }
            else
            {
                cout << "fail" << endl;
            }
        }

        cout << endl;
    }

    for (int k = 0; k < i; k++)
    {
        failFlag[0] = false;
        failFlag[1] = false;
        if (pollard2(rop, num_arr[k]))
        {
            mpz_set(res_arr[0], rop);
            mpz_cdiv_q(res_arr[1], num_arr[k], rop);
            for (int x = 0; x < 2; x++)
            {

            returnPoint:
                mpz_sqrt(rop, res_arr[x]);
                mpz_set_str(two, "2", 10);
                while (mpz_cmp(two, rop) <= 0)
                {
                    if (mpz_divisible_p(res_arr[x], two) != 0)
                    {
                        // gmp_printf("%Zd\n", two);
                        printer(two, false, false);
                        mpz_cdiv_q(res_arr[x], res_arr[x], two);
                        // flag[k] = true;
                        goto returnPoint; //find other factors
                    }
                    else
                    {
                        mpz_add_ui(two, two, 1);
                    }

                    if (mpz_cmp(two, limit) >= 0)
                    {
                        // for (int j = k; j < i; j++)
                        // {
                        //     cout << endl
                        //          << "fail" << endl;
                        // }
                        // // cout << "time" << (double)(clock() - tStart) / CLOCKS_PER_SEC << endl;
                        // return 0;
                        // cout<<"keri"<<endl;
                        printer(two, false, true);
                        failFlag[x] = true;
                        break;
                    }
                }

                if (!failFlag[x])
                {
                    // gmp_printf("%Zd\n", num_arr[k]);
                    printer(res_arr[x], true, false);
                    // if ((double)(clock() - tStart) / CLOCKS_PER_SEC > 14.5)
                    // {

                    //     for (int j = k + 1; j < i; j++)
                    //     {
                    //         cout
                    //             << "fail" << endl
                    //             << endl;
                    //     }
                    //     // cout << "time" << (double)(clock() - tStart) / CLOCKS_PER_SEC << endl;
                    //     return 0;
                    // }
                }
                else
                {
                    // if ((double)(clock() - tStart) / CLOCKS_PER_SEC > 14.5)
                    // {

                    //     for (int j = k; j < i; j++)
                    //     {
                    //         cout
                    //             << "fail" << endl
                    //             << endl;
                    //     }
                    //     // cout << "time" << (double)(clock() - tStart) / CLOCKS_PER_SEC << endl;
                    //     return 0;
                    // }
                }
            }
        }
        else
        {
            gmp_printf("%Zd\n", num_arr[k]);
        }

        cout << endl;
    }
    return 0;
}