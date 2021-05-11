#include "interfaces/highs_c_api.h"

#include <stdio.h>
#include <stdlib.h>
// Force asserts to be checked always.
#undef NDEBUG
#include <assert.h>

void minimal_api() {
  HighsInt numcol = 2;
  HighsInt numrow = 2;
  HighsInt nnz = 4;
  HighsInt i;

  double cc[2] = {1.0, -2.0};
  double cl[2] = {0.0, 0.0};
  double cu[2] = {10.0, 10.0};
  double rl[2] = {0.0, 0.0};
  double ru[2] = {2.0, 1.0};
  HighsInt astart[3] = {0, 2, 4};
  HighsInt aindex[4] = {0, 1, 0, 1};
  double avalue[4] = {1.0, 2.0, 1.0, 3.0};

  double* cv = (double*)malloc(sizeof(double) * numcol);
  double* cd = (double*)malloc(sizeof(double) * numcol);
  double* rv = (double*)malloc(sizeof(double) * numrow);
  double* rd = (double*)malloc(sizeof(double) * numrow);

  HighsInt* cbs = (HighsInt*)malloc(sizeof(HighsInt) * numcol);
  HighsInt* rbs = (HighsInt*)malloc(sizeof(HighsInt) * numrow);

  int modelstatus;

  int status = Highs_call(numcol, numrow, nnz, cc, cl, cu, rl, ru, astart, aindex, avalue, cv,
            cd, rv, rd, cbs, rbs, &modelstatus);
  assert(status == 0);

  for (i = 0; i < numcol; i++) {
    printf("x%"HIGHSINT_FORMAT" = %lf\n", i, cv[i]);
  }

  free(cv);
  free(cd);
  free(rv);
  free(rd);
  free(cbs);
  free(rbs);
}

void minimal_api_lp() {
  // This illustrates the use of Highs_call, the simple C interface to
  // HiGHS. It's designed to solve the general LP problem
  //
  // Min c^Tx subject to L <= Ax <= U; l <= x <= u
  //
  // where A is a matrix with m rows and n columns
  //
  // The scalar n is numcol
  // The scalar m is numrow
  //
  // The vector c is colcost
  // The vector l is collower
  // The vector u is colupper
  // The vector L is rowlower
  // The vector U is rowupper
  //
  // The matrix A is represented in packed column-wise form: only its
  // nonzeros are stored
  //
  // * The number of nonzeros in A is numnz
  //
  // * The row indices of the nonnzeros in A are stored column-by-column
  // in aindex
  //
  // * The values of the nonnzeros in A are stored column-by-column in
  // avalue
  //
  // * The position in aindex/avalue of the index/value of the first
  // nonzero in each column is stored in astart
  //
  // Note that astart[0] must be zero
  //
  // After a successful call to Highs_call, the primal and dual
  // solution, and the simplex basis are returned as follows
  //
  // The vector x is colvalue
  // The vector Ax is rowvalue
  // The vector of dual values for the variables x is coldual
  // The vector of dual values for the variables Ax is rowdual
  // The basic/nonbasic status of the variables x is colbasisstatus
  // The basic/nonbasic status of the variables Ax is rowbasisstatus
  //
  // The status of the solution obtained is modelstatus
  //
  // To solve maximization problems, the values in c must be negated
  //
  // The use of Highs_call is illustrated for the LP
  //
  // Min    f  = 2x_0 + 3x_1
  // s.t.                x_1 <= 6
  //       10 <=  x_0 + 2x_1 <= 14
  //        8 <= 2x_0 +  x_1
  // 0 <= x_0 <= 3; 1 <= x_1

  const int numcol = 2;
  const int numrow = 3;
  const int numnz = 5;

  // Define the column costs, lower bounds and upper bounds
  double colcost[2] = {2.0, 3.0};
  double collower[2] = {0.0, 1.0};
  double colupper[2] = {3.0, 1.0e30};
  // Define the row lower bounds and upper bounds
  double rowlower[3] = {-1.0e30, 10.0, 8.0};
  double rowupper[3] = {6.0, 14.0, 1.0e30};
  // Define the constraint matrix column-wise
  int astart[2] = {0, 2};
  int aindex[5] = {1, 2, 0, 1, 2};
  double avalue[5] = {1.0, 2.0, 1.0, 2.0, 1.0};

  double* colvalue = (double*)malloc(sizeof(double) * numcol);
  double* coldual = (double*)malloc(sizeof(double) * numcol);
  double* rowvalue = (double*)malloc(sizeof(double) * numrow);
  double* rowdual = (double*)malloc(sizeof(double) * numrow);

  int* colbasisstatus = (int*)malloc(sizeof(int) * numcol);
  int* rowbasisstatus = (int*)malloc(sizeof(int) * numrow);

  int modelstatus;

  int runstatus = Highs_call(numcol, numrow, numnz,
			  colcost, collower, colupper, rowlower, rowupper,
			  astart, aindex, avalue,
			  colvalue, coldual, rowvalue, rowdual,
			  colbasisstatus, rowbasisstatus,
			  &modelstatus);

  assert(runstatus == 0);

  printf("Run status = %d; Model status = %d\n", runstatus, modelstatus);

  int i;
  if (modelstatus == 7) {
    double objective_value = 0;
    // Report the column primal and dual values, and basis status
    for (i = 0; i < numcol; i++) {
      printf("Col%d = %lf; dual = %lf; status = %d; \n", i, colvalue[i], coldual[i], colbasisstatus[i]);
      objective_value += colvalue[i]*colcost[i];
    }
    // Report the row primal and dual values, and basis status
    for (i = 0; i < numrow; i++) {
      printf("Row%d = %lf; dual = %lf; status = %d; \n", i, rowvalue[i], rowdual[i], rowbasisstatus[i]);
    }
    printf("Optimal objective value = %g\n", objective_value);
  }

  free(colvalue);
  free(coldual);
  free(rowvalue);
  free(rowdual);
  free(colbasisstatus);
  free(rowbasisstatus);
}

void full_api() {
  void* highs;

  highs = Highs_create();

  double cc[2] = {1.0, -2.0};
  double cl[2] = {0.0, 0.0};
  double cu[2] = {10.0, 10.0};
  double rl[2] = {0.0, 0.0};
  double ru[2] = {2.0, 1.0};
  HighsInt astart[3] = {0, 2, 4};
  HighsInt aindex[4] = {0, 1, 0, 1};
  double avalue[4] = {1.0, 2.0, 1.0, 3.0};

  assert( Highs_addCols(highs, 2, cc, cl, cu, 0, NULL, NULL, NULL) );
  assert( Highs_addRows(highs, 2, rl, ru,  4, astart, aindex, avalue) );

  Highs_run(highs);
  Highs_destroy(highs);
}

void full_api_lp() {
  // Form and solve the LP
  // Min    f  = 2x_0 + 3x_1
  // s.t.                x_1 <= 6
  //       10 <=  x_0 + 2x_1 <= 14
  //        8 <= 2x_0 +  x_1
  // 0 <= x_0 <= 3; 1 <= x_1

  void* highs;

  highs = Highs_create();

  const int numcol = 2;
  const int numrow = 3;
  const int numnz = 5;
  int i;

  // Define the column costs, lower bounds and upper bounds
  double colcost[2] = {2.0, 3.0};
  double collower[2] = {0.0, 1.0};
  double colupper[2] = {3.0, 1.0e30};
  // Define the row lower bounds and upper bounds
  double rowlower[3] = {-1.0e30, 10.0, 8.0};
  double rowupper[3] = {6.0, 14.0, 1.0e30};
  // Define the constraint matrix row-wise, as it is added to the LP
  // with the rows
  int arstart[3] = {0, 1, 3};
  int arindex[5] = {1, 0, 1, 0, 1};
  double arvalue[5] = {1.0, 1.0, 2.0, 2.0, 1.0};

  double* colvalue = (double*)malloc(sizeof(double) * numcol);
  double* coldual = (double*)malloc(sizeof(double) * numcol);
  double* rowvalue = (double*)malloc(sizeof(double) * numrow);
  double* rowdual = (double*)malloc(sizeof(double) * numrow);

  int* colbasisstatus = (int*)malloc(sizeof(int) * numcol);
  int* rowbasisstatus = (int*)malloc(sizeof(int) * numrow);

  // Add two columns to the empty LP
  assert( Highs_addCols(highs, numcol, colcost, collower, colupper, 0, NULL, NULL, NULL) );
  // Add three rows to the 2-column LP
  assert( Highs_addRows(highs, numrow, rowlower, rowupper, numnz, arstart, arindex, arvalue) );

  int sense;
  Highs_getObjectiveSense(highs, &sense);
  printf("LP problem has objective sense = %d\n", sense);
  assert(sense == 1);

  sense *= -1;
  Highs_changeObjectiveSense(highs, sense);
  assert(sense == -1);

  sense *= -1;
  Highs_changeObjectiveSense(highs, sense);

  Highs_getObjectiveSense(highs, &sense);
  printf("LP problem has old objective sense = %d\n", sense);
  assert(sense == 1);

  int simplex_scale_strategy;
  Highs_getIntOptionValue(highs, "simplex_scale_strategy", &simplex_scale_strategy);
  printf("simplex_scale_strategy = %d: setting it to 3\n", simplex_scale_strategy);
  simplex_scale_strategy = 3;
  Highs_setIntOptionValue(highs, "simplex_scale_strategy", simplex_scale_strategy);

  // There are some functions to check what type of option value you should
  // provide.
  int option_type;
  int ret;
  ret = Highs_getOptionType(highs, "simplex_scale_strategy", &option_type);
  assert(ret == 0);
  assert(option_type == 1);
  ret = Highs_getOptionType(highs, "bad_option", &option_type);
  assert(ret != 0);

  double primal_feasibility_tolerance;
  Highs_getDoubleOptionValue(highs, "primal_feasibility_tolerance", &primal_feasibility_tolerance);
  printf("primal_feasibility_tolerance = %g: setting it to 1e-6\n", primal_feasibility_tolerance);
  primal_feasibility_tolerance = 1e-6;
  Highs_setDoubleOptionValue(highs, "primal_feasibility_tolerance", primal_feasibility_tolerance);

  Highs_setHighsBoolOptionValue(highs, "output_flag", 0);
  printf("Running quietly...\n");
  int runstatus = Highs_run(highs);
  printf("Running loudly...\n");
  Highs_setHighsBoolOptionValue(highs, "output_flag", 1);

  // Get the model status
  int modelstatus = Highs_getModelStatus(highs);

  printf("Run status = %d; Model status = %d = %s\n", runstatus, modelstatus, Highs_modelStatusToChar(highs, modelstatus));

  double objective_function_value;
  Highs_getDoubleInfoValue(highs, "objective_function_value", &objective_function_value);
  int simplex_iteration_count = 0;
  Highs_getIntInfoValue(highs, "simplex_iteration_count", &simplex_iteration_count);
  int primal_solution_status = 0;
  Highs_getIntInfoValue(highs, "primal_solution_status", &primal_solution_status);
  int dual_solution_status = 0;
  Highs_getIntInfoValue(highs, "dual_solution_status", &dual_solution_status);

  printf("Objective value = %g; Iteration count = %d\n", objective_function_value, simplex_iteration_count);
  if (modelstatus == 7) {
    printf("Solution primal status = %s\n", Highs_solutionStatusToChar(highs, primal_solution_status));
    printf("Solution dual status = %s\n", Highs_solutionStatusToChar(highs, dual_solution_status));
    // Get the primal and dual solution
    Highs_getSolution(highs, colvalue, coldual, rowvalue, rowdual);
    // Get the basis
    Highs_getBasis(highs, colbasisstatus, rowbasisstatus);
    // Report the column primal and dual values, and basis status
    for (i = 0; i < numcol; i++) {
      printf("Col%d = %lf; dual = %lf; status = %d; \n", i, colvalue[i], coldual[i], colbasisstatus[i]);
    }
    // Report the row primal and dual values, and basis status
    for (i = 0; i < numrow; i++) {
      printf("Row%d = %lf; dual = %lf; status = %d; \n", i, rowvalue[i], rowdual[i], rowbasisstatus[i]);
    }
  }

  free(colvalue);
  free(coldual);
  free(rowvalue);
  free(rowdual);
  free(colbasisstatus);
  free(rowbasisstatus);

  Highs_destroy(highs);

  // Define the constraint matrix col-wise to pass to the LP
  int rowwise = 0;
  int astart[2] = {0, 2};
  int aindex[5] = {1, 2, 0, 1, 2};
  double avalue[5] = {1.0, 2.0, 1.0, 2.0, 1.0};
  highs = Highs_create();
  runstatus = Highs_passLp(highs, numcol, numrow, numnz, rowwise,
			colcost, collower, colupper,
			rowlower, rowupper,
			astart, aindex, avalue);
  runstatus = Highs_run(highs);
  modelstatus = Highs_getModelStatus(highs);
  printf("Run status = %d; Model status = %d = %s\n", runstatus, modelstatus, Highs_modelStatusToChar(highs, modelstatus));
  Highs_destroy(highs);
}

void options() {
  void* highs = Highs_create();

  HighsInt simplex_scale_strategy;
  Highs_setIntOptionValue(highs, "simplex_scale_strategy", 0);
  Highs_getIntOptionValue(highs, "simplex_scale_strategy", &simplex_scale_strategy);
  assert( simplex_scale_strategy == 0 );

  double primal_feasibility_tolerance;
  Highs_setDoubleOptionValue(highs, "primal_feasibility_tolerance", 2.0);
  Highs_getDoubleOptionValue(highs, "primal_feasibility_tolerance", &primal_feasibility_tolerance);
  assert( primal_feasibility_tolerance == 2.0 );

  Highs_destroy(highs);
}

void test_getColsByRange() {
    void* highs = Highs_create();
    Highs_addCol(highs, -1.0, 0.0, 1.0, 0, NULL, NULL);
    Highs_addCol(highs, -1.0, 0.0, 1.0, 0, NULL, NULL);
    HighsInt aindex[2] = {0, 1};
    double avalue[2] = {1.0, -1.0};
    Highs_addRow(highs, 0.0, 0.0, 2, aindex, avalue);
    HighsInt num_cols;
    HighsInt num_nz;
    HighsInt matrix_start[2] = {-1, -1};
    Highs_getColsByRange(highs, 0, 1, &num_cols, NULL, NULL, NULL, &num_nz,
                         matrix_start, NULL, NULL);
    assert( num_cols == 2 );
    assert( num_nz == 2 );
    assert( matrix_start[0] == 0 );
    assert( matrix_start[1] == 1 );
    HighsInt matrix_indices[2] = {-1, -1};
    double matrix_values[2] = {0.0, 0.0};
    Highs_getColsByRange(highs, 0, 1, &num_cols, NULL, NULL, NULL, &num_nz,
                         matrix_start, matrix_indices, matrix_values);
    assert( matrix_indices[0] == 0 );
    assert( matrix_indices[1] == 0 );
    assert( matrix_values[0] == 1.0 );
    assert( matrix_values[1] == -1.0 );
    Highs_destroy(highs);
}

int main() {
  minimal_api();
  full_api();
  minimal_api_lp();
  full_api_lp();
  options();
  test_getColsByRange();
  return 0;
}
