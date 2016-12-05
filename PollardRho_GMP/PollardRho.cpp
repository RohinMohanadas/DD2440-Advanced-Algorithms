#include <iostream>
#include <malloc.h>
#include <gmp.h>
#include <ctime>

using namespace std;

//  This function takes in an integer as input and sets the value of ret
//  to one of its factors. This factor is not necessarily a prime factor
bool pollard2(mpz_t ret, mpz_t Number)
{
    int counter = 0;
    mpz_t x, y, p, d, randLimit, rop;
    mpz_init(x);
    mpz_init(y);
    mpz_init(p);
    mpz_init(d);
    mpz_init(rop);
    mpz_set_str(x, "2", 10); //  x is 2 initially : this is one avenue of introducing randomness

    mpz_mul(rop, x, x);        //  x^2
    mpz_add_ui(rop, rop, 1);   //  x^2 + 1
    mpz_mod(rop, rop, Number); //  f(x) = (x^2 + 1) mod N
    mpz_set(y, rop);           //  y = f(x)

    mpz_sub(rop, x, y);
    mpz_abs(rop, rop);
    mpz_gcd(p, rop, Number); //  gcd(|x-y|)

    while (mpz_cmp_ui(p, 1) == 0)
    {
        if (counter >= 100000) //  Limiter used to prevent Pollard from running infinitely
        {
            // probable prime
            mpz_set_str(ret, "0", 10);
            return false;
        }

        mpz_mul(x, x, x);
        mpz_add_ui(x, x, 1);   //  rop = f(x)
        mpz_mod(x, x, Number); //  tortoise

        mpz_mul(y, y, y);
        mpz_add_ui(y, y, 1);
        mpz_mod(y, y, Number);
        mpz_mul(y, y, y);
        mpz_add_ui(y, y, 1);   //  rop = f(f(x))
        mpz_mod(y, y, Number); //  rabbit

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

//  Buffering and printing function. The function will store the factors passed to it
//  If while computing a factor, limit is breached or the time limit is exceeded, then
//  flush variable is set to true, in which case, it disregards all the stored factors
//  and sets the global index variable to 0. print variable is set to true when all the
//  prime factors have been found and only then the prime factors are sent to the std op
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

//  The naive factorization method. Takes 2 mpz_t arrays and two counter variables as parameters
//  naive_ip array contains the factors of an integer as output by Pollard Rho algorithm. Also, a
//  Limiter is set to limit the max size of a prime factor so that naive algorithm does not run for
//  more than 15 seconds which is the allowed time limit.
void naive(mpz_t *naive_ip, mpz_t *naive_op, int ipCtr, int opCtr)
{
    bool failFlag = false;
    mpz_t rop, divisor, limit;
    mpz_init(rop);
    mpz_init(divisor);
    mpz_init(limit);
    mpz_set_str(divisor, "2", 10);
    mpz_set_str(limit, "3999999", 10);
    int i = 0;
    for (i = 0; i <= ipCtr; i++)
    {
        if (mpz_probab_prime_p(naive_ip[i], 50) == 2)
        {
            printer(naive_ip[i], naive_op, false, false);
        }
        else
        {
        //  add the prime to list
        returnPoint:
            mpz_sqrt(rop, naive_ip[i]);
            mpz_set_str(divisor, "2", 10);
            while (mpz_cmp(divisor, rop) <= 0)
            {
                if (mpz_divisible_p(naive_ip[i], divisor) != 0)
                {

                    printer(divisor, naive_op, false, false);
                    mpz_cdiv_q(naive_ip[i], naive_ip[i], divisor);
                    goto returnPoint; //find other factors
                }
                else
                {
                    mpz_add_ui(divisor, divisor, 1);
                }
                if (mpz_cmp(divisor, limit) >= 0)
                {
                    printer(divisor, naive_op, false, true);
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

//  Execution starts here
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
    }

    for (int j = 0; j < i; j++)
    {
        int ipCtr = 0;
        int opCtr = 0;

        //  Execute pollard till it returns false. i.e. till there are possible factors.
        while (pollard2(ret, num_arr[j]))
        {
            mpz_init2(naive_ip[ipCtr], bits);
            mpz_set(naive_ip[ipCtr++], ret);

            mpz_cdiv_q(num_arr[j], num_arr[j], ret);
            if (mpz_probab_prime_p(num_arr[j], 50) == 2)
                break;
        }

        mpz_init2(naive_ip[ipCtr], bits);
        mpz_set(naive_ip[ipCtr], num_arr[j]);

        naive(naive_ip, naive_op, ipCtr, opCtr);

        cout << endl;
    }
    return 0;
}