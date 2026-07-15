#include "rosalind.h"

/*
Tom, who in the 0th generation has genotype Aa Bb.
Each organism always mates with an organism having genotype Aa Bb.
Each organism always has two children.

Get the probability that at least N Aa Bb organisms will belong to the k-th generation of Tom's family tree.
P(p<=N) = sum (n = 0 to N) (P(p=n))

Aa Bb x Aa Bb = (1/2 Aa + 1/4 AA + 1/4 aa) x (1/2 Bb + 1/4 BB + 1/4 bb) = [1/4 Aa Bb] + ...
Aa Bb x Aa BB = (1/2 Aa + 1/4 AA + 1/4 aa) x (1/2 Bb + 1/2 BB + 0 bb) = [1/4 Aa Bb] + ...
Aa Bb x Aa bb = (1/2 Aa + 1/4 AA + 1/4 aa) x (1/2 Bb + 0 BB + 1/2 bb) = [1/4 Aa Bb] + ...
Aa Bb x AA Bb = (1/2 Aa + 1/2 AA + 0 aa) x (1/2 Bb + 1/4 BB + 1/4 bb) = [1/4 Aa Bb] + ...
Aa Bb x AA BB = (1/2 Aa + 1/2 AA + 0 aa) x (1/2 Bb + 1/2 BB + 0 bb) = [1/4 Aa Bb] + ...
Aa Bb x AA bb = (1/2 Aa + 1/2 AA + 0 aa) x (1/2 Bb + 0 BB + 1/2 bb) = [1/4 Aa Bb] + ...
Aa Bb x aa Bb = (1/2 Aa + 0 AA + 1/2 aa) x (1/2 Bb + 1/4 BB + 1/4 bb) = [1/4 Aa Bb] + ...
Aa Bb x aa BB = (1/2 Aa + 0 AA + 1/2 aa) x (1/2 Bb + 1/2 BB + 0 bb) = [1/4 Aa Bb] + ...
Aa Bb x aa bb = (1/2 Aa + 0 AA + 1/2 aa) x (1/2 Bb + 0 BB + 1/2 bb) = [1/4 Aa Bb] + ...


*/
static double
lcomb (unsigned long n, unsigned long p) {
  assert (p <= n);
  if (2 * p > n)
    p = n - p;
  double C = 0.;
  for (unsigned long i = 0; i < p; i++)
    C += log ((double)(n - i)) - log ((double)(i + 1));
  return C;
}

static int
eqf (double a, double b) {
  static const double epsilon = 1e-6;
  return fabs (a - b) < epsilon;
}

// Run with LD_LIBRARY_PATH=.::../minimaps:../aho-corasick-1975
int
main (void) {
  setlocale (LC_ALL, "");
  static char *genotype_order[3] = { "AA", "Aa", "aa" };
  /*
          Aa x
         parent
c       AA Aa  aa
h  AA (1/2 1/4  0 )
i  Aa (1/2 1/2 1/2)
l  aa ( 0  1/4 1/2)
d
  */
  const double p_Aa[3][3] = { { .5, .25, .0 },
                              { .5, .5, .5 },
                              { .0, .25, .5 } };
  for (size_t j = 0; j < lengthof (p_Aa[0]); j++) {
    double tot = .0;
    for (size_t i = 0; i < lengthof (p_Aa); i++)
      tot += p_Aa[i][j];
    assert (eqf (tot, 1.));
  }

  fprintf (stderr, "             Aa x\n     ");
  for (size_t i = 0; i < lengthof (p_Aa); i++)
    fprintf (stderr, "  %s   ", genotype_order[i]);
  fprintf (stderr, "\n");
  for (size_t i = 0; i < lengthof (p_Aa); i++) {
    fprintf (stderr, "%s  ", genotype_order[i]);
    for (size_t j = 0; j < lengthof (p_Aa[0]); j++)
      fprintf (stderr, "%.4f ", p_Aa[i][j]);
    fprintf (stderr, "\n");
  }

  for (size_t j = 0; j < lengthof (p_Aa[1]); j++)
    assert (eqf (p_Aa[1][j], 0.5)); // Whatever the genotype of the mate, probablity of having one Aa offspring = 1/2.
  // From p_Aa, we see that, whatever the genotype of the mate, Probablity of having one Aa offspring = 1/2

  /* Aa Bb x parent */
  double p_Aa_Bb[lengthof (p_Aa) * lengthof (p_Aa)][lengthof (p_Aa[0]) * lengthof (p_Aa[0])];
  for (size_t j = 0; j < lengthof (p_Aa_Bb[0]); j++) {
    double tot = .0;
    for (size_t i = 0; i < lengthof (p_Aa_Bb); i++)
      tot += (p_Aa_Bb[i][j] = p_Aa[i / lengthof (p_Aa)][j / lengthof (p_Aa)] * p_Aa[i % lengthof (p_Aa)][j % lengthof (p_Aa)]);
    assert (eqf (tot, 1.));
  }

  fprintf (stderr, "                                Aa Bb x\n      ");
  for (size_t i = 0; i < lengthof (p_Aa_Bb[0]); i++)
    fprintf (stderr, " %s %c%c ", genotype_order[i / lengthof (p_Aa)],
             isupper (genotype_order[i % lengthof (p_Aa)][0]) ? 'B' : 'b',
             isupper (genotype_order[i % lengthof (p_Aa)][1]) ? 'B' : 'b');
  fprintf (stderr, "\n");
  for (size_t i = 0; i < lengthof (p_Aa_Bb); i++) {
    fprintf (stderr, "%s %c%c  ", genotype_order[i / lengthof (p_Aa)],
             isupper (genotype_order[i % lengthof (p_Aa)][0]) ? 'B' : 'b',
             isupper (genotype_order[i % lengthof (p_Aa)][1]) ? 'B' : 'b');
    for (size_t j = 0; j < lengthof (p_Aa_Bb[0]); j++)
      fprintf (stderr, "%.4f ", p_Aa_Bb[i][j]);
    fprintf (stderr, "\n");
  }

  for (size_t j = 0; j < lengthof (p_Aa_Bb[4]); j++)
    assert (eqf (p_Aa_Bb[4][j], 0.25)); // Whatever the genotype of the mate, probablity of having one Aa Bb offspring = 1/4.
  const double pAaBb = .25;             // Probability of having offspring Aa Bb with mate Aa Bb (whatever the genotype of the other parent).

  /* From p_Aa_Bb, we see that, whatever the genotype of the mate, probablity of having one Aa Bb offspring = 1/4
     Probablity of having two Aa Bb offspring out of two = P2 = (Probablity of having one Aa Bb offspring) x (Probablity of having one Aa Bb offspring) = 1/4 x 1/4 = 1/16
     Probablity of having zero Aa Bb offspring out of two = P0 = (Probablity of having other than Aa Bb offspring) x (Probablity of having other than Aa Bb offspring) = 3/4 x 3/4 = 9/16
     Probablity of having exactly one Aa Bb offspring out of two = P1 = 1 - P0 - P2 = 3/8 */

  /*
                 Prob of N° of Aa Bb
 Gen Pop         0                  1                2       3      4
  0   1          1
  1   2        9/16               3/8             1/16
  2   4      9/16*9/16        2*9/16*3/8     2*1/4*3/8 + 3/4*3/4 4*(
  */

  /*
  Population P = 2^(k-1) on the (k-1)th generation.
  Each organism can have 0 (prob 9/16), 1 (prob 3/8) or 2 (prob 1/16) Aa Bb offsprings.
  P0 have 0, P1 have 1, P2 have 2, P0+P1+P2=P.
  For each (P0,P1,P-P0-P1) probability of this combination is:
  [(9/16)^P0 * (3/8)^P1 * (1/16)^P2] * P!/(P-P0)!/P0! * (P-P0)!/(P-P0-P1)!/P1!
  P! = P0! * (P0+P1)!/P0! * P!/(P0+P1)!
  Population of Aa Bb = P1 + 2*P2, with P1 + 2*P2 <= N
  */

  wchar_t *line = 0;
  assert (fgetwcs (&line, 0, stdin));

  size_t k = 2; // generation
  size_t N = 1; // at least N Aa Bb organisms will belong to the k-th generation

  wchar_t *token, *endtoken;
  wchar_t *state = line;
  assert ((token = wcstok (0, L" \t\n", &state)));
  k = wcstoull (token, &endtoken, 10);
  assert (endtoken > token);
  assert ((token = wcstok (0, L" \t\n", &state)));
  N = wcstoull (token, &endtoken, 10);
  assert (endtoken > token);
  fprintf (stderr, "%zu %zu\n", k, N);

  free (line);

  size_t population;
  double proba_at_least_N;
  // Second try, simpler and faster
  population = 1; // Tom is in the 0th generation
  for (size_t i = 1; i <= k; i++)
    population *= 2; // at k th generation, 2 offsprings per organism.
  assert (N <= population);
  proba_at_least_N = .0;
  for (size_t nAaBb = N; nAaBb <= population; nAaBb++)
    proba_at_least_N += exp (log (pAaBb) * (double)nAaBb + lcomb (population, nAaBb) + log (1 - pAaBb) * (double)(population - nAaBb));
  fprintf (stdout, "%.3f\n", proba_at_least_N);

#ifdef FIRST_TRY
  // First try
  population = 1; // Tom is in the 0th generation
  for (size_t i = 1; i <= k - 1; i++)
    population *= 2; // at (k-1)th generation, 2 offsprings per organism.
  assert (N <= 2 * population);

  const double lp0 = log (1. - pAaBb) + log (1. - pAaBb);       // Probability of having 0 offsprings Aa Bb out of 2.
  const double lp1 = log (2.) + log (pAaBb) + log (1. - pAaBb); // Probability of having exactly 1 offspring Aa Bb out of 2.
  const double lp2 = log (pAaBb) + log (pAaBb);                 // Probability of having 2 offsprings Aa Bb out of 2.

  proba_at_least_N = .0;
  double proba_all = .0;
  for (size_t P1 = 0; P1 <= population; P1++)        // population having exactly one offspring Aa Bb.
    for (size_t P2 = 0; P1 + P2 <= population; P2++) // population having exactly two offsprings Aa Bb.
    {
      double lprob = 0.;
      size_t P0 = population - P1 - P2; // population having exactly zero offsprings Aa Bb.
      lprob += (double)P0 * lp0;
      lprob += lcomb (population, P0);
      lprob += (double)P1 * lp1;
      lprob += lcomb (population - P0, P1);
      lprob += (double)P2 * lp2;
      lprob += lcomb (population - P0 - P1, P2);
      proba_all += exp (lprob);
      if (1 /* offspring Aa Bb */ * P1 + 2 /* offsprings Aa Bb */ * P2 >= N /* at least N Aa Bb organisms will belong to the k-th generation */)
        proba_at_least_N += exp (lprob);
    }
  fprintf (stderr, "%g\n", proba_all);
  assert (eqf (proba_all, 1.));
  fprintf (stderr, "%.3f\n", proba_at_least_N);
#endif
}
