#include "simint/simint_eri.h"
#include "simint/ostei/ostei.h"

int simint_compute_eri(struct simint_multi_shellpair const * P,
                       struct simint_multi_shellpair const * Q,
                       double screen_tol,
                       double * restrict work,
                       double * restrict integrals)
{
    return simint_compute_ostei(P, Q, screen_tol, work, integrals);
}

int simint_compute_eri_deriv(int deriv,
                             struct simint_multi_shellpair const * P,
                             struct simint_multi_shellpair const * Q,
                             double screen_tol,
                             double * restrict work,
                             double * restrict integrals)
{
    return simint_compute_ostei_deriv(deriv, P, Q, screen_tol, work, integrals);
}


size_t simint_eri_worksize(int derorder, int maxam)
{
    return simint_ostei_worksize(derorder, maxam);
}


size_t simint_eri_workmem(int derorder, int maxam)
{
    return simint_ostei_workmem(derorder, maxam);
}
