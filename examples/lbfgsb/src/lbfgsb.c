#include "lbfgsb.h"
static integer c__1 = 1;


/*
 L-BFGS-B is released under the "New BSD License" (aka "Modified BSD License" 
 or "3-clause license") 
 Please read attached file License.txt 

===========   L-BFGS-B (version 3.0.  April 25, 2011  =================== 

    This is a modified version of L-BFGS-B. Minor changes in the updated 
    code appear preceded by a line comment as follows 

    c-jlm-jn 

    Major changes are described in the accompanying paper: 

        Jorge Nocedal and Jose Luis Morales, Remark on "Algorithm 778: 
        L-BFGS-B: Fortran Subroutines for Large-Scale Bound Constrained 
        Optimization"  (2011). To appear in  ACM Transactions on 
        Mathematical Software, 

    The paper describes an improvement and a correction to Algorithm 778. 
    It is shown that the performance of the algorithm can be improved 
    significantly by making a relatively simple modication to the subspace 
    minimization phase. The correction concerns an error caused by the use 
    of routine dpmeps to estimate machine precision. 

    The total work space **wa** required by the new version is 

                 2*m*n + 11m*m + 5*n + 8*m 

    the old version required 

                 2*m*n + 12m*m + 4*n + 12*m 


           J. Nocedal  Department of Electrical Engineering and 
                       Computer Science. 
                       Northwestern University. Evanston, IL. USA 


          J.L Morales  Departamento de Matematicas, 
                       Instituto Tecnologico Autonomo de Mexico 
                       Mexico D.F. Mexico. 

                       March  2011 
   */

/* ============================================================================= */
/* Subroutine */ int setulb(integer *n, integer *m, double *x, 
	double *l, double *u, integer *nbd, double *f, double 
	*g, double *factr, double *pgtol, double *wa, integer *
	iwa, integer *task, integer *iprint, integer *csave, logical *lsave, 
	integer *isave, double *dsave) /* ftnlen task_len, ftnlen csave_len) */
{
    /* System generated locals */
    integer i__1;


    /* Local variables */
    static integer ld, lr, lt, lz, lwa, lwn, lss, lxp, lws, lwt, lsy, lwy, 
	    lsnd;

/* -jlm-jn */
    /*
    ************ 

    Subroutine setulb 

    This subroutine partitions the working arrays wa and iwa, and 
      then uses the limited memory BFGS method to solve the bound 
      constrained optimization problem by calling mainlb. 
      (The direct method will be used in the subspace minimization.) 

    n is an integer variable. 
      On entry n is the dimension of the problem. 
      On exit n is unchanged. 

    m is an integer variable. 
      On entry m is the maximum number of variable metric corrections 
        used to define the limited memory matrix. 
      On exit m is unchanged. 

    x is a double precision array of dimension n. 
      On entry x is an approximation to the solution. 
      On exit x is the current approximation. 

    l is a double precision array of dimension n. 
      On entry l is the lower bound on x. 
      On exit l is unchanged. 

    u is a double precision array of dimension n. 
      On entry u is the upper bound on x. 
      On exit u is unchanged. 

    nbd is an integer array of dimension n. 
      On entry nbd represents the type of bounds imposed on the 
        variables, and must be specified as follows: 
        nbd(i)=0 if x(i) is unbounded, 
               1 if x(i) has only a lower bound, 
               2 if x(i) has both lower and upper bounds, and 
               3 if x(i) has only an upper bound. 
      On exit nbd is unchanged. 

    f is a double precision variable. 
      On first entry f is unspecified. 
      On final exit f is the value of the function at x. 

    g is a double precision array of dimension n. 
      On first entry g is unspecified. 
      On final exit g is the value of the gradient at x. 

    factr is a double precision variable. 
      On entry factr >= 0 is specified by the user.  The iteration 
        will stop when 

        (f^k - f^{k+1})/max{|f^k|,|f^{k+1}|,1} <= factr*epsmch 

        where epsmch is the machine precision, which is automatically 
        generated by the code. Typical values for factr: 1.d+12 for 
        low accuracy; 1.d+7 for moderate accuracy; 1.d+1 for extremely 
        high accuracy. 
      On exit factr is unchanged. 

    pgtol is a double precision variable. 
      On entry pgtol >= 0 is specified by the user.  The iteration 
        will stop when 

                max{|proj g_i | i = 1, ..., n} <= pgtol 

        where pg_i is the ith component of the projected gradient. 
      On exit pgtol is unchanged. 

    wa is a double precision working array of length 
      (2mmax + 5)nmax + 12mmax^2 + 12mmax. 

    iwa is an integer working array of length 3nmax. 

    task is a working string of characters of length 60 indicating 
      the current job when entering and quitting this subroutine. 

    iprint is an integer variable that must be set by the user. 
      It controls the frequency and type of output generated: 
       iprint<0    no output is generated; 
       iprint=0    print only one line at the last iteration; 
       0<iprint<99 print also f and |proj g| every iprint iterations; 
       iprint=99   print details of every iteration except n-vectors; 
       iprint=100  print also the changes of active set and final x; 
       iprint>100  print details of every iteration including x and g; 
      When iprint > 0, the file iterate.dat will be created to 
                       summarize the iteration. 

    csave is a working string of characters of length 60. 

    lsave is a logical working array of dimension 4. 
      On exit with 'task' = NEW_X, the following information is 
                                                            available: 
        If lsave(1) = .true.  then  the initial X has been replaced by 
                                    its projection in the feasible set; 
        If lsave(2) = .true.  then  the problem is constrained; 
        If lsave(3) = .true.  then  each variable has upper and lower 
                                    bounds; 

    isave is an integer working array of dimension 44. 
      On exit with 'task' = NEW_X, the following information is 
                                                            available: 
        isave(22) = the total number of intervals explored in the 
                        search of Cauchy points; 
        isave(26) = the total number of skipped BFGS updates before 
                        the current iteration; 
        isave(30) = the number of current iteration; 
        isave(31) = the total number of BFGS updates prior the current 
                        iteration; 
        isave(33) = the number of intervals explored in the search of 
                        Cauchy point in the current iteration; 
        isave(34) = the total number of function and gradient 
                        evaluations; 
        isave(36) = the number of function value or gradient 
                                 evaluations in the current iteration; 
        if isave(37) = 0  then the subspace argmin is within the box; 
        if isave(37) = 1  then the subspace argmin is beyond the box; 
        isave(38) = the number of free variables in the current 
                        iteration; 
        isave(39) = the number of active constraints in the current 
                        iteration; 
        n + 1 - isave(40) = the number of variables leaving the set of 
                          active constraints in the current iteration; 
        isave(41) = the number of variables entering the set of active 
                        constraints in the current iteration. 

    dsave is a double precision working array of dimension 29. 
      On exit with 'task' = NEW_X, the following information is 
                                                            available: 
        dsave(1) = current 'theta' in the BFGS matrix; 
        dsave(2) = f(x) in the previous iteration; 
        dsave(3) = factr*epsmch; 
        dsave(4) = 2-norm of the line search direction vector; 
        dsave(5) = the machine precision epsmch generated by the code; 
        dsave(7) = the accumulated time spent on searching for 
                                                        Cauchy points; 
        dsave(8) = the accumulated time spent on 
                                                subspace minimization; 
        dsave(9) = the accumulated time spent on line search; 
        dsave(11) = the slope of the line search function at 
                                 the current point of line search; 
        dsave(12) = the maximum relative step length imposed in 
                                                          line search; 
        dsave(13) = the infinity norm of the projected gradient; 
        dsave(14) = the relative step length in the line search; 
        dsave(15) = the slope of the line search function at 
                                the starting point of the line search; 
        dsave(16) = the square of the 2-norm of the line search 
                                                     direction vector. 

    Subprograms called: 

      L-BFGS-B Library ... mainlb. 


    References: 

      [1] R. H. Byrd, P. Lu, J. Nocedal and C. Zhu, ``A limited 
      memory algorithm for bound constrained optimization'', 
      SIAM J. Scientific Computing 16 (1995), no. 5, pp. 1190--1208. 

      [2] C. Zhu, R.H. Byrd, P. Lu, J. Nocedal, ``L-BFGS-B: a 
      limited memory FORTRAN code for solving bound constrained 
      optimization problems'', Tech. Report, NAM-11, EECS Department, 
      Northwestern University, 1994. 

      (Postscript files of these papers are available via anonymous 
       ftp to eecs.nwu.edu in the directory pub/lbfgs/lbfgs_bcm.) 

                          *  *  * 

    NEOS, November 1994. (Latest revision June 1996.) 
    Optimization Technology Center. 
    Argonne National Laboratory and Northwestern University. 
    Written by 
                       Ciyou Zhu 
    in collaboration with R.H. Byrd, P. Lu-Chen and J. Nocedal. 


    ************ 
    */
/* -jlm-jn */
    /* Parameter adjustments */
    --iwa;
    --g;
    --nbd;
    --u;
    --l;
    --x;
    --wa;
    --lsave;
    --isave;
    --dsave;

    /* Function Body */
    if ( *task == START ) {
        isave[1] = *m * *n;
        /* Computing 2nd power */
        i__1 = *m;
        isave[2] = i__1 * i__1;
        /* Computing 2nd power */
        i__1 = *m;
        isave[3] = i__1 * i__1 << 2;
        isave[4] = 1;
        /* ws      m*n */
        isave[5] = isave[4] + isave[1];
        /* wy      m*n */
        isave[6] = isave[5] + isave[1];
        /* wsy     m**2 */
        isave[7] = isave[6] + isave[2];
        /* wss     m**2 */
        isave[8] = isave[7] + isave[2];
        /* wt      m**2 */
        isave[9] = isave[8] + isave[2];
        /* wn      4*m**2 */
        isave[10] = isave[9] + isave[3];
        /* wsnd    4*m**2 */
        isave[11] = isave[10] + isave[3];
        /* wz      n */
        isave[12] = isave[11] + *n;
        /* wr      n */
        isave[13] = isave[12] + *n;
        /* wd      n */
        isave[14] = isave[13] + *n;
        /* wt      n */
        isave[15] = isave[14] + *n;
        /* wxp     n */
        isave[16] = isave[15] + *n;
        /* wa      8*m */
    }
    lws = isave[4];
    lwy = isave[5];
    lsy = isave[6];
    lss = isave[7];
    lwt = isave[8];
    lwn = isave[9];
    lsnd = isave[10];
    lz = isave[11];
    lr = isave[12];
    ld = isave[13];
    lt = isave[14];
    lxp = isave[15];
    lwa = isave[16];
    mainlb(n, m, &x[1], &l[1], &u[1], &nbd[1], f, &g[1], factr, pgtol, &wa[
            lws], &wa[lwy], &wa[lsy], &wa[lss], &wa[lwt], &wa[lwn], &wa[lsnd],
            &wa[lz], &wa[lr], &wa[ld], &wa[lt], &wa[lxp], &wa[lwa], &iwa[1], 
            &iwa[*n + 1], &iwa[(*n << 1) + 1], task, iprint, csave, &lsave[1],
            &isave[22], &dsave[1]); /* (ftnlen)60, (ftnlen)60); */
    return 0;
} /* setulb_ */



/* Table of constant values */

static double c_b7 = 0.;
/* SRB: note that task_len is no longer used since task
 * is now integer* not char*.
 * Similarly for csave (now integer*) and so csave_len not used */

/* Subroutine */ int mainlb(integer *n, integer *m, double *x, 
	double *l, double *u, integer *nbd, double *f, double 
	*g, double *factr, double *pgtol, double *ws, double *
	wy, double *sy, double *ss, double *wt, double *wn, 
	double *snd, double *z__, double *r__, double *d__, 
	double *t, double *xp, double *wa, integer *index, 
	integer *iwhere, integer *indx2, integer *task, integer *iprint, 
    integer *csave, logical *lsave, integer *isave, double *dsave) /*(ftnlen ) */
/* 	task_len, ftnlen csave_len) */
{

    /* System generated locals */
    integer ws_dim1, ws_offset, wy_dim1, wy_offset, sy_dim1, sy_offset, 
	    ss_dim1, ss_offset, wt_dim1, wt_offset, wn_dim1, wn_offset, 
	    snd_dim1, snd_offset, i__1=0;
    double d__1, d__2;
    fileType o__1=NULL;

    /* Local variables */
    static integer i__, k;
    static double gd, dr, rr, dtd;
    static integer col;
    static double tol;
    static logical wrk;
    static double stp, cpu1, cpu2;
    static integer head;
    static double fold;
    static integer nact;
    static double ddum;
    static integer info, nseg;
    static double time;
    static integer nfgv, ifun, iter;
    static integer wordTemp;
    static integer *word=&wordTemp;
    static double time1, time2;
    static integer iback;
    static double gdold;
    static integer nfree;
    static logical boxed;
    static integer itail;
    static double theta;
    static double dnorm;
    static integer nskip, iword;
    static double xstep, stpmx;
    static integer ileave;
    static double cachyt;
    static integer itfile;
    static double epsmch;
    static logical updatd;
    static double sbtime;
    static logical prjctd;
    static integer iupdat;
    static double sbgnrm;
    static logical cnstnd;
    static integer nenter;
    static double lnscht;
    static integer nintol;

/* -jlm-jn */
/*     ************ */

/*
    Subroutine mainlb 

    This subroutine solves bound constrained optimization problems by 
      using the compact formula of the limited memory BFGS updates. 

    n is an integer variable. 
      On entry n is the number of variables. 
      On exit n is unchanged. 

    m is an integer variable. 
      On entry m is the maximum number of variable metric 
         corrections allowed in the limited memory matrix. 
      On exit m is unchanged. 

    x is a double precision array of dimension n. 
      On entry x is an approximation to the solution. 
      On exit x is the current approximation. 

    l is a double precision array of dimension n. 
      On entry l is the lower bound of x. 
      On exit l is unchanged. 

    u is a double precision array of dimension n. 
      On entry u is the upper bound of x. 
      On exit u is unchanged. 

    nbd is an integer array of dimension n. 
      On entry nbd represents the type of bounds imposed on the 
        variables, and must be specified as follows: 
        nbd(i)=0 if x(i) is unbounded, 
               1 if x(i) has only a lower bound, 
               2 if x(i) has both lower and upper bounds, 
               3 if x(i) has only an upper bound. 
      On exit nbd is unchanged. 

    f is a double precision variable. 
      On first entry f is unspecified. 
      On final exit f is the value of the function at x. 

    g is a double precision array of dimension n. 
      On first entry g is unspecified. 
      On final exit g is the value of the gradient at x. 

    factr is a double precision variable. 
      On entry factr >= 0 is specified by the user.  The iteration 
        will stop when 

        (f^k - f^{k+1})/max{|f^k|,|f^{k+1}|,1} <= factr*epsmch 

        where epsmch is the machine precision, which is automatically 
        generated by the code. 
      On exit factr is unchanged. 

    pgtol is a double precision variable. 
      On entry pgtol >= 0 is specified by the user.  The iteration 
        will stop when 

                max{|proj g_i | i = 1, ..., n} <= pgtol 

        where pg_i is the ith component of the projected gradient. 
      On exit pgtol is unchanged. 

    ws, wy, sy, and wt are double precision working arrays used to 
      store the following information defining the limited memory 
         BFGS matrix: 
         ws, of dimension n x m, stores S, the matrix of s-vectors; 
         wy, of dimension n x m, stores Y, the matrix of y-vectors; 
         sy, of dimension m x m, stores S'Y; 
         ss, of dimension m x m, stores S'S; 
         yy, of dimension m x m, stores Y'Y; 
         wt, of dimension m x m, stores the Cholesky factorization 
                                 of (theta*S'S+LD^(-1)L'); see eq. 
                                 (2.26) in [3]. 

    wn is a double precision working array of dimension 2m x 2m 
      used to store the LEL^T factorization of the indefinite matrix 
                K = [-D -Y'ZZ'Y/theta     L_a'-R_z'  ] 
                    [L_a -R_z           theta*S'AA'S ] 

      where     E = [-I  0] 
                    [ 0  I] 

    snd is a double precision working array of dimension 2m x 2m 
      used to store the lower triangular part of 
                N = [Y' ZZ'Y   L_a'+R_z'] 
                    [L_a +R_z  S'AA'S   ] 

    z(n),r(n),d(n),t(n), xp(n),wa(8*m) are double precision working arrays. 
      z  is used at different times to store the Cauchy point and 
         the Newton point. 
      xp is used to safeguard the projected Newton direction 

    sg(m),sgo(m),yg(m),ygo(m) are double precision working arrays. 

    index is an integer working array of dimension n. 
      In subroutine freev, index is used to store the free and fixed 
         variables at the Generalized Cauchy Point (GCP). 

    iwhere is an integer working array of dimension n used to record 
      the status of the vector x for GCP computation. 
      iwhere(i)=0 or -3 if x(i) is free and has bounds, 
                1       if x(i) is fixed at l(i), and l(i) .ne. u(i) 
                2       if x(i) is fixed at u(i), and u(i) .ne. l(i) 
                3       if x(i) is always fixed, i.e.,  u(i)=x(i)=l(i) 
               -1       if x(i) is always free, i.e., no bounds on it. 

    indx2 is an integer working array of dimension n. 
      Within subroutine cauchy, indx2 corresponds to the array iorder. 
      In subroutine freev, a list of variables entering and leaving 
      the free set is stored in indx2, and it is passed on to 
      subroutine formk with this information. 

    task is a working string of characters of length 60 indicating 
      the current job when entering and leaving this subroutine. 

    iprint is an INTEGER variable that must be set by the user. 
      It controls the frequency and type of output generated: 
       iprint<0    no output is generated; 
       iprint=0    print only one line at the last iteration; 
       0<iprint<99 print also f and |proj g| every iprint iterations; 
       iprint=99   print details of every iteration except n-vectors; 
       iprint=100  print also the changes of active set and final x; 
       iprint>100  print details of every iteration including x and g; 
      When iprint > 0, the file iterate.dat will be created to 
                       summarize the iteration. 

    csave is a working string of characters of length 60. 

    lsave is a logical working array of dimension 4. 

    isave is an integer working array of dimension 23. 

    dsave is a double precision working array of dimension 29. 


    Subprograms called 

      L-BFGS-B Library ... cauchy, subsm, lnsrlb, formk, 

       errclb, prn1lb, prn2lb, prn3lb, active, projgr, 

       freev, cmprlb, matupd, formt. 

      Minpack2 Library ... timer 

      Linpack Library ... dcopy, ddot. 


    References: 

      [1] R. H. Byrd, P. Lu, J. Nocedal and C. Zhu, ``A limited 
      memory algorithm for bound constrained optimization'', 
      SIAM J. Scientific Computing 16 (1995), no. 5, pp. 1190--1208. 

      [2] C. Zhu, R.H. Byrd, P. Lu, J. Nocedal, ``L-BFGS-B: FORTRAN 
      Subroutines for Large Scale Bound Constrained Optimization'' 
      Tech. Report, NAM-11, EECS Department, Northwestern University, 
      1994. 

      [3] R. Byrd, J. Nocedal and R. Schnabel "Representations of 
      Quasi-Newton Matrices and their use in Limited Memory Methods'', 
      Mathematical Programming 63 (1994), no. 4, pp. 129-156. 

      (Postscript files of these papers are available via anonymous 
       ftp to eecs.nwu.edu in the directory pub/lbfgs/lbfgs_bcm.) 

                          *  *  * 

    NEOS, November 1994. (Latest revision June 1996.) 
    Optimization Technology Center. 
    Argonne National Laboratory and Northwestern University. 
    Written by 
                       Ciyou Zhu 
    in collaboration with R.H. Byrd, P. Lu-Chen and J. Nocedal. 
    */


/*     ************ */
    /* Parameter adjustments */
    --indx2;
    --iwhere;
    --index;
    --xp;
    --t;
    --d__;
    --r__;
    --z__;
    --g;
    --nbd;
    --u;
    --l;
    --x;
    --wa;
    snd_dim1 = 2 * *m;
    snd_offset = 1 + snd_dim1;
    snd -= snd_offset;
    wn_dim1 = 2 * *m;
    wn_offset = 1 + wn_dim1;
    wn -= wn_offset;
    wt_dim1 = *m;
    wt_offset = 1 + wt_dim1;
    wt -= wt_offset;
    ss_dim1 = *m;
    ss_offset = 1 + ss_dim1;
    ss -= ss_offset;
    sy_dim1 = *m;
    sy_offset = 1 + sy_dim1;
    sy -= sy_offset;
    wy_dim1 = *n;
    wy_offset = 1 + wy_dim1;
    wy -= wy_offset;
    ws_dim1 = *n;
    ws_offset = 1 + ws_dim1;
    ws -= ws_offset;
    --lsave;
    --isave;
    --dsave;

    /* Function Body */
    if ( *task == START ) {
        epsmch = DBL_EPSILON;
        timer(&time1);
        /*        Initialize counters and scalars when task='START'. */
        /*           for the limited memory BFGS matrices: */
        col = 0;
        head = 1;
        theta = 1.;
        iupdat = 0;
        updatd = FALSE_;
        iback = 0;
        itail = 0;
        iword = 0;
        nact = 0;
        ileave = 0;
        nenter = 0;
        fold = 0.;
        dnorm = 0.;
        cpu1 = 0.;
        gd = 0.;
        stpmx = 0.;
        sbgnrm = 0.;
        stp = 0.;
        gdold = 0.;
        dtd = 0.;
        /*           for operation counts: */
        iter = 0;
        nfgv = 0;
        nseg = 0;
        nintol = 0;
        nskip = 0;
        nfree = *n;
        ifun = 0;
        /*           for stopping tolerance: */
        tol = *factr * epsmch;
        /*           for measuring running time: */
        cachyt = 0.;
        sbtime = 0.;
        lnscht = 0.;
        /*           'word' records the status of subspace solutions. */
        *word = WORD_DEFAULT;
/*         s_copy(word, "---", (ftnlen)3, (ftnlen)3); */
        /*           'info' records the termination information. */
        info = 0;
        itfile = 8;
        /* Note: no longer trying to write to file */
        /*        Check the input arguments for errors. */
        errclb(n, m, factr, &l[1], &u[1], &nbd[1], task, &info, &k, (ftnlen)
                60);
        if ( IS_ERROR(*task) ){
            prn3lb(n, &x[1], f, task, iprint, &info, o__1, &iter, &nfgv, &
                    nintol, &nskip, &nact, &sbgnrm, &c_b7, &nseg, word, &
                    iback, &stp, &xstep, &k, &cachyt, &sbtime, &lnscht, (
                        ftnlen)60, (ftnlen)3);
            return 0;
        }
        prn1lb(n, m, &l[1], &u[1], &x[1], iprint, o__1, &epsmch); 
        active(n, &l[1], &u[1], &nbd[1], &x[1], &iwhere[1], iprint, &prjctd, 
                &cnstnd, &boxed);
        /*        The end of the initialization. */
    } else {
        /*          restore local variables. */
        prjctd = lsave[1];
        cnstnd = lsave[2];
        boxed = lsave[3];
        updatd = lsave[4];
        nintol = isave[1];
        itfile = isave[3];
        iback = isave[4];
        nskip = isave[5];
        head = isave[6];
        col = isave[7];
        itail = isave[8];
        iter = isave[9];
        iupdat = isave[10];
        nseg = isave[12];
        nfgv = isave[13];
        info = isave[14];
        ifun = isave[15];
        iword = isave[16];
        nfree = isave[17];
        nact = isave[18];
        ileave = isave[19];
        nenter = isave[20];
        theta = dsave[1];
        fold = dsave[2];
        tol = dsave[3];
        dnorm = dsave[4];
        epsmch = dsave[5];
        cpu1 = dsave[6];
        cachyt = dsave[7];
        sbtime = dsave[8];
        lnscht = dsave[9];
        time1 = dsave[10];
        gd = dsave[11];
        stpmx = dsave[12];
        sbgnrm = dsave[13];
        stp = dsave[14];
        gdold = dsave[15];
        dtd = dsave[16];
        /*        After returning from the driver go to the point where execution */
        /*        is to resume. */
        if ( *task == FG_LN ) {
            goto L666;
        }
        if ( *task == NEW_X ) {
            goto L777;
        }
        if ( *task == FG_ST ) {
            goto L111;
        }
        if ( IS_STOP(*task) ) {
            if ( *task == STOP_CPU ) {
                /*    restore the previous iterate. */
                dcopy(n, &t[1], &c__1, &x[1], &c__1);
                dcopy(n, &r__[1], &c__1, &g[1], &c__1);
                *f = fold;
            }
            goto L999;
        }
    }
    /*     Compute f0 and g0. */
    *task = FG_START;
    /*    return to the driver to calculate f and g; reenter at 111. */
    goto L1000;
L111:
    nfgv = 1;
    /*     Compute the infinity norm of the (-) projected gradient. */
    projgr(n, &l[1], &u[1], &nbd[1], &x[1], &g[1], &sbgnrm);
    if (*iprint >= 1) {
        printf("At iterate %5ld, f(x)= %15.8f, ||proj grad||_infty = %.2e\n",iter,*f,sbgnrm );
    }
    if (sbgnrm <= *pgtol) {
        /*    terminate the algorithm. */
        *task = CONV_GRAD;
        goto L999;
    }
    /* ----------------- the beginning of the loop -------------------------- */
L222:
    if (*iprint >= 99) {
        printf("ITERATION %5ld\n", i__1 );
    }
    iword = -1;

    if (! cnstnd && col > 0) {
        /*    skip the search for GCP. */
        dcopy(n, &x[1], &c__1, &z__[1], &c__1);
        wrk = updatd;
        nseg = 0;
        goto L333;
    }
    /* ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc */

    /*     Compute the Generalized Cauchy Point (GCP). */

    /* ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc */
    timer(&cpu1);
    info = 0;
    cauchy(n, &x[1], &l[1], &u[1], &nbd[1], &g[1], &indx2[1], &iwhere[1], &t[
            1], &d__[1], &z__[1], m, &wy[wy_offset], &ws[ws_offset], &sy[
            sy_offset], &wt[wt_offset], &theta, &col, &head, &wa[1], &wa[(*m 
                << 1) + 1], &wa[(*m << 2) + 1], &wa[*m * 6 + 1], &nseg, iprint, &
            sbgnrm, &info, &epsmch);
    if (info != 0) {
        /*         singular triangular system detected; refresh the lbfgs memory. */
        info = 0;
        col = 0;
        head = 1;
        theta = 1.;
        iupdat = 0;
        updatd = FALSE_;
        timer(&cpu2);
        cachyt = cachyt + cpu2 - cpu1;
        goto L222;
    }
    timer(&cpu2);
    cachyt = cachyt + cpu2 - cpu1;
    nintol += nseg;
    /*     Count the entering and leaving variables for iter > 0; */
    /*     find the index set of free and active variables at the GCP. */
    freev(n, &nfree, &index[1], &nenter, &ileave, &indx2[1], &iwhere[1], &
            wrk, &updatd, &cnstnd, iprint, &iter);
    nact = *n - nfree;
L333:
    /*     If there are no free variables or B=theta*I, then */
    /*                                        skip the subspace minimization. */
    if (nfree == 0 || col == 0) {
        goto L555;
    }
    /* ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc */

    /*     Subspace minimization. */

    /* ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc */
    timer(&cpu1);
    /*     Form  the LEL^T factorization of the indefinite */
    /*       matrix    K = [-D -Y'ZZ'Y/theta     L_a'-R_z'  ] */
    /*                     [L_a -R_z           theta*S'AA'S ] */
    /*       where     E = [-I  0] */
    /*                     [ 0  I] */
    if (wrk) {
        formk(n, &nfree, &index[1], &nenter, &ileave, &indx2[1], &iupdat, &
                updatd, &wn[wn_offset], &snd[snd_offset], m, &ws[ws_offset], &
                wy[wy_offset], &sy[sy_offset], &theta, &col, &head, &info);
    }
    if (info != 0) {
        /*          nonpositive definiteness in Cholesky factorization; */
        /*          refresh the lbfgs memory and restart the iteration. */
        if (*iprint >= 1) {
            printf(" Nonpositive definiteness in Cholesky factorization in formk;\n");
            printf("   refresh the lbfgs memory and restart the iteration.\n");
        }
        info = 0;
        col = 0;
        head = 1;
        theta = 1.;
        iupdat = 0;
        updatd = FALSE_;
        timer(&cpu2);
        sbtime = sbtime + cpu2 - cpu1;
        goto L222;
    }
    /*        compute r=-Z'B(xcp-xk)-Z'g (using wa(2m+1)=W'(xcp-x) */
    /*                                                   from 'cauchy'). */
    cmprlb(n, m, &x[1], &g[1], &ws[ws_offset], &wy[wy_offset], &sy[sy_offset]
            , &wt[wt_offset], &z__[1], &r__[1], &wa[1], &index[1], &theta, &
            col, &head, &nfree, &cnstnd, &info);
    if (info != 0) {
        goto L444;
    }
    /* -jlm-jn   call the direct method. */
    subsm(n, m, &nfree, &index[1], &l[1], &u[1], &nbd[1], &z__[1], &r__[1], &
            xp[1], &ws[ws_offset], &wy[wy_offset], &theta, &x[1], &g[1], &col,
            &head, &iword, &wa[1], &wn[wn_offset], iprint, &info);
L444:
    if (info != 0) {
        /*          singular triangular system detected; */
        /*          refresh the lbfgs memory and restart the iteration. */
        if (*iprint >= 1) {
            printf(" Singular triangular system detected;\n");
            printf("   refresh the lbfgs memory and restart the iteration.\n");
        }
        info = 0;
        col = 0;
        head = 1;
        theta = 1.;
        iupdat = 0;
        updatd = FALSE_;
        timer(&cpu2);
        sbtime = sbtime + cpu2 - cpu1;
        goto L222;
    }
    timer(&cpu2);
    sbtime = sbtime + cpu2 - cpu1;
L555:
    /* ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc */

    /*     Line search and optimality tests. */

    /* ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc */
    /*     Generate the search direction d:=z-x. */
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
        d__[i__] = z__[i__] - x[i__];
        /* L40: */
    }
    timer(&cpu1);
L666:
    lnsrlb(n, &l[1], &u[1], &nbd[1], &x[1], f, &fold, &gd, &gdold, &g[1], &
            d__[1], &r__[1], &t[1], &z__[1], &stp, &dnorm, &dtd, &xstep, &
            stpmx, &iter, &ifun, &iback, &nfgv, &info, task, &boxed, &cnstnd, 
            csave, &isave[22], &dsave[17]); /* (ftnlen)60, (ftnlen)60); */
    if (info != 0 || iback >= 20) {
        /*          restore the previous iterate. */
        dcopy(n, &t[1], &c__1, &x[1], &c__1);
        dcopy(n, &r__[1], &c__1, &g[1], &c__1);
        *f = fold;
        if (col == 0) {
            /*             abnormal termination. */
            if (info == 0) {
                info = -9;
                /*                restore the actual number of f and g evaluations etc. */
                --nfgv;
                --ifun;
                --iback;
            }
            *task = ABNORMAL;
            ++iter;
            goto L999;
        } else {
            /*             refresh the lbfgs memory and restart the iteration. */
            if (*iprint >= 1) {
                printf(" Bad direction in the line search;\n");
                printf("   refresh the lbfgs memory and restart the iteration.\n");
            }
            if (info == 0) {
                --nfgv;
            }
            info = 0;
            col = 0;
            head = 1;
            theta = 1.;
            iupdat = 0;
            updatd = FALSE_;
            *task = RESTART;
            timer(&cpu2);
            lnscht = lnscht + cpu2 - cpu1;
            goto L222;
        }
    } else if ( *task == FG_LN ) {
        /*          return to the driver for calculating f and g; reenter at 666. */
        goto L1000;
    } else {
        /*          calculate and print out the quantities related to the new X. */
        timer(&cpu2);
        lnscht = lnscht + cpu2 - cpu1;
        ++iter;
        /*        Compute the infinity norm of the projected (-)gradient. */
        projgr(n, &l[1], &u[1], &nbd[1], &x[1], &g[1], &sbgnrm);
        /*        Print iteration information. */



        prn2lb(n, &x[1], f, &g[1], iprint, o__1, &iter, &nfgv, &nact, &
                sbgnrm, &nseg, word, &iword, &iback, &stp, &xstep, (ftnlen)3);
        goto L1000;
    }
L777:
    /*     Test for termination. */
    if (sbgnrm <= *pgtol) {
        /*                                terminate the algorithm. */
        *task = CONV_GRAD;
        goto L999;
    }
    /* Computing MAX */
    d__1 = abs(fold), d__2 = abs(*f), d__1 = max(d__1,d__2);
    ddum = max(d__1,1.);
    if (fold - *f <= tol * ddum) {
        /*                                        terminate the algorithm. */
        *task = CONV_F;
        if (iback >= 10) {
            info = -5;
        }
        /*           i.e., to issue a warning if iback>10 in the line search. */
        goto L999;
    }
    /*     Compute d=newx-oldx, r=newg-oldg, rr=y'y and dr=y's. */
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
        r__[i__] = g[i__] - r__[i__];
        /* L42: */
    }
    rr = ddot(n, &r__[1], &c__1, &r__[1], &c__1);
    if (stp == 1.) {
        dr = gd - gdold;
        ddum = -gdold;
    } else {
        dr = (gd - gdold) * stp;
        dscal(n, &stp, &d__[1], &c__1);
        ddum = -gdold * stp;
    }
    if (dr <= epsmch * ddum) {
        /*                            skip the L-BFGS update. */
        ++nskip;
        updatd = FALSE_;
        if (*iprint >= 1) {
            printf("  ys=%10.3e  -gs=%10.3e BFGS update SKIPPED\n", dr, ddum );
        }
        goto L888;
    }
    /* ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc */

    /*     Update the L-BFGS matrix. */

    /* ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc */
    updatd = TRUE_;
    ++iupdat;
    /*     Update matrices WS and WY and form the middle matrix in B. */
    matupd(n, m, &ws[ws_offset], &wy[wy_offset], &sy[sy_offset], &ss[
            ss_offset], &d__[1], &r__[1], &itail, &iupdat, &col, &head, &
            theta, &rr, &dr, &stp, &dtd);
    /*     Form the upper half of the pds T = theta*SS + L*D^(-1)*L'; */
    /*        Store T in the upper triangular of the array wt; */
    /*        Cholesky factorize T to J*J' with */
    /*           J' stored in the upper triangular of wt. */
    formt(m, &wt[wt_offset], &sy[sy_offset], &ss[ss_offset], &col, &theta, &
            info);
    if (info != 0) {
        /*          nonpositive definiteness in Cholesky factorization; */
        /*          refresh the lbfgs memory and restart the iteration. */
        if (*iprint >= 1) {
            printf(" Nonpositive definiteness in Cholesky factorization in formt;\n");
            printf("   refresh the lbfgs memory and restart the iteration.\n");
        }
        info = 0;
        col = 0;
        head = 1;
        theta = 1.;
        iupdat = 0;
        updatd = FALSE_;
        goto L222;
    }
    /*     Now the inverse of the middle matrix in B is */
    /*       [  D^(1/2)      O ] [ -D^(1/2)  D^(-1/2)*L' ] */
    /*       [ -L*D^(-1/2)   J ] [  0        J'          ] */
L888:
    /* -------------------- the end of the loop ----------------------------- */
    goto L222;
L999:
    timer(&time2);
    time = time2 - time1;
    prn3lb(n, &x[1], f, task, iprint, &info, o__1, &iter, &nfgv, &nintol, 
            &nskip, &nact, &sbgnrm, &time, &nseg, word, &iback, &stp, &xstep, 
            &k, &cachyt, &sbtime, &lnscht, (ftnlen)60, (ftnlen)3);
L1000:
    /*     Save local variables. */
    lsave[1] = prjctd;
    lsave[2] = cnstnd;
    lsave[3] = boxed;
    lsave[4] = updatd;
    isave[1] = nintol;
    isave[3] = itfile;
    isave[4] = iback;
    isave[5] = nskip;
    isave[6] = head;
    isave[7] = col;
    isave[8] = itail;
    isave[9] = iter;
    isave[10] = iupdat;
    isave[12] = nseg;
    isave[13] = nfgv;
    isave[14] = info;
    isave[15] = ifun;
    isave[16] = iword;
    isave[17] = nfree;
    isave[18] = nact;
    isave[19] = ileave;
    isave[20] = nenter;
    dsave[1] = theta;
    dsave[2] = fold;
    dsave[3] = tol;
    dsave[4] = dnorm;
    dsave[5] = epsmch;
    dsave[6] = cpu1;
    dsave[7] = cachyt;
    dsave[8] = sbtime;
    dsave[9] = lnscht;
    dsave[10] = time1;
    dsave[11] = gd;
    dsave[12] = stpmx;
    dsave[13] = sbgnrm;
    dsave[14] = stp;
    dsave[15] = gdold;
    dsave[16] = dtd;
    return 0;
} /* mainlb */
