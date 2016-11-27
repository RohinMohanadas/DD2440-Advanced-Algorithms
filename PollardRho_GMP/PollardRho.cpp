#include <iostream>
#include <malloc.h>
#include <gmp.h>
#include <ctime>

using namespace std;

void pollard(mpz_t ret, mpz_t Number, gmp_randstate_t state)
{
    mpz_t x, y, c, d, randLimit, rop;
    mpz_init(x);
    mpz_init(y);
    mpz_init(c);
    mpz_init(d);
    mpz_init(randLimit);
    mpz_init(rop);

    mpz_set_str(randLimit, "999998", 10);
    if (mpz_divisible_ui_p(Number, 2))
    {
        mpz_set_str(ret, "2", 10);
    }
    else
    {
        cout << "test1" << endl;
        gmp_randinit_mt(state);
        mpz_urandomm(x, state, randLimit);
        cout << "testblah" << endl;
        mpz_add_ui(x, x, 2); //obtained random x
        gmp_printf("x is %Zd\n", x);
        mpz_set(y, x); //x=y
        cout << "testblah2" << endl;
        gmp_randinit_mt(state);
        mpz_urandomm(c, state, randLimit);
        mpz_add_ui(c, c, 2); //obtained random c
        gmp_printf("c is %Zd\n", c);
        mpz_set_str(d, "1", 10);
        cout << "test2" << endl;
        while (mpz_cmp_ui(d, 1) == 0)
        {
            mpz_mul(x, x, x);
            mpz_add(x, x, c);
            mpz_mod(x, x, Number); // hare

            mpz_mul(y, y, y);
            mpz_add(y, y, c);
            mpz_mod(y, y, Number);
            mpz_mul(y, y, y);
            mpz_add(y, y, c);
            mpz_mod(y, y, Number); // tortoise

            mpz_sub(rop, x, y);
            mpz_abs(rop, rop);
            mpz_gcd(d, rop, Number);
            if (mpz_cmp(d, Number) == 0)
            {
                break;
            }
        }
        cout << "test3" << endl;
        mpz_set(ret, rop);
    }
}
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

int indx = 0;

void printer(mpz_t factor, mpz_t *naive_op, bool print, bool flush)
{

    if (flush == true)
    {
        indx = 0;
        cout << "fail"
             << endl;
        // cout << "Here as well" << endl;
    }
    else if (print == true)
    {
        // mpz_init2(naive_op[indx], 100);
        // mpz_set(naive_op[indx++], factor);
        for (int i = 0; i < indx; i++)
        {
            gmp_printf("%Zd\n", naive_op[i]);
        }

        indx = 0;
    }
    else
    {
        mpz_init2(naive_op[indx], 100);
        mpz_set(naive_op[indx++], factor);
    }
}

void naive(mpz_t *naive_ip, mpz_t *naive_op, int ipCtr, int opCtr)
{
    bool failFlag = false;
    mpz_t rop, two, limit;
    mpz_init(rop);
    mpz_init(two);
    mpz_init(limit);
    mpz_set_str(two, "2", 10);
    mpz_set_str(limit, "7999999", 10);
    int i = 0;
    for (i = 0; i <= ipCtr; i++)
    {
        // cout << "Test 1 and ipctr is" <<ipCtr<< endl;
        if (mpz_probab_prime_p(naive_ip[i], 50) == 2)
        {
            // cout << "Test 2" << endl;
            printer(naive_ip[i], naive_op, false, false);
        }
        else
        {
        //  add the prime to list
        // gmp_printf("Test 3 %Zd\n", naive_ip[i]);
        returnPoint:
            mpz_sqrt(rop, naive_ip[i]);
            mpz_set_str(two, "2", 10);
            while (mpz_cmp(two, rop) <= 0)
            {
                if (mpz_divisible_p(naive_ip[i], two) != 0)
                {
                    // gmp_printf("%Zd\n", two);
                    printer(two, naive_op, false, false);
                    mpz_cdiv_q(naive_ip[i], naive_ip[i], two);
                    // flag[k] = true;
                    goto returnPoint; //find other factors
                }
                else
                {
                    mpz_add_ui(two, two, 1);
                }
                if (mpz_cmp(two, limit) >= 0)
                {
                    // cout << "Here" << endl;
                    printer(two, naive_op, false, true);
                    failFlag = true;
                    break;
                }
            }
            if (!failFlag)
                printer(naive_ip[i], naive_op, false, false);
        }
    }
    if (!failFlag)
    {
        printer(naive_ip[i], naive_op, true, false);
    }
    return;
}
int main()
{
    int count = 1000, bits = 100, i = 0;
    mpz_t ret, z;
    mpz_init(ret);
    mpz_init(z);

    mpz_t *num_arr;
    mpz_t *naive_ip;
    mpz_t *naive_op;
    num_arr = (mpz_t *)malloc(count * sizeof(mpz_t));
    naive_ip = (mpz_t *)malloc(100 * sizeof(mpz_t));
    naive_op = (mpz_t *)malloc(100 * sizeof(mpz_t));
    num_arr = (mpz_t *)malloc(count * sizeof(mpz_t));
    while (gmp_scanf("%Zd", z) == 1)
    {
        mpz_init2(num_arr[i], bits);
        mpz_set(num_arr[i++], z);
        // cout << "Scan :" << i << endl;
    }

    for (int j = 0; j < i; j++)
    {
        int ipCtr = 0;
        int opCtr = 0;

        while (pollard2(ret, num_arr[j]))
        {
            // gmp_printf("%Zd\n", ret);
            mpz_init2(naive_ip[ipCtr], bits);
            mpz_set(naive_ip[ipCtr++], ret);

            mpz_cdiv_q(num_arr[j], num_arr[j], ret);
            if (mpz_probab_prime_p(num_arr[j], 50) == 2)
                break;
        }

        // gmp_printf("%Zd\n", z);
        mpz_init2(naive_ip[ipCtr], bits);
        mpz_set(naive_ip[ipCtr], num_arr[j]);
        // cout << "Number for Pollard factors: " << ipCtr + 1 << endl;
        naive(naive_ip, naive_op, ipCtr, opCtr);

        cout<<endl;
    }
    //for
    return 0;
}