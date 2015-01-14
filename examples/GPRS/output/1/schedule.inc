WELSPECS
--     WELL   GROUP LOCATION  BHP   PI
--     NAME   NAME    I  J   DEPTH DEFN
       'prod1'   'grp1'     -1  -1    1*  'WAT'  /
       'prod2'   'grp1'     -1  -1    1*  'WAT'  /
       'inj1'   'grp1'     -1  -1    1*  'WAT'  /

 / 

COMPDAT
--     WELL     -LOCATION- OPEN/ SAT CONN  WELL
--     NAME     I  J K1 K2 SHUT  TAB FACT  DIAM
       'prod1'     -1  -1  -1  -1  'OPEN' 2*       0.3/
       'prod2'     -1  -1  -1  -1  'OPEN' 2*       0.3/
       'inj1'     -1  -1  -1  -1  'OPEN' 2*       0.3/

 / 

-- CONTROL INTERVAL 1
WCONPROD
--      WELL     OPEN/  CNTL   OIL  WATER   GAS  LIQU   RES   BHP
--      NAME     SHUT   MODE  RATE   RATE  RATE  RATE  RATE
        'prod1'    'OPEN'  'BHP'   5*                            2300/
        'prod2'    'OPEN'  'BHP'   5*                            2300/
/ 

WCONINJE
--      WELL     INJ   OPEN/   CNTL    FLOW   RES    BHP
--      NAME    TYPE   SHUT    MODE    RATE   RATE  TARGET
        'inj1'   'WATER' 'OPEN' 'RATE'   5000/
/

TSTEP
0.5 1 2 4 8 16 32 36.5 
/

-- CONTROL INTERVAL 2
WCONPROD
--      WELL     OPEN/  CNTL   OIL  WATER   GAS  LIQU   RES   BHP
--      NAME     SHUT   MODE  RATE   RATE  RATE  RATE  RATE
        'prod1'    'OPEN'  'BHP'   5*                            2000/
        'prod2'    'OPEN'  'BHP'   5*                            2000/
/ 

WCONINJE
--      WELL     INJ   OPEN/   CNTL    FLOW   RES    BHP
--      NAME    TYPE   SHUT    MODE    RATE   RATE  TARGET
        'inj1'   'WATER' 'OPEN' 'RATE'   5000/
/

TSTEP
0.5 1 2 4 8 16 32 50 50 36.5 
/

-- CONTROL INTERVAL 3
WCONPROD
--      WELL     OPEN/  CNTL   OIL  WATER   GAS  LIQU   RES   BHP
--      NAME     SHUT   MODE  RATE   RATE  RATE  RATE  RATE
        'prod1'    'OPEN'  'BHP'   5*                            2000/
        'prod2'    'OPEN'  'BHP'   5*                            2000/
/ 

WCONINJE
--      WELL     INJ   OPEN/   CNTL    FLOW   RES    BHP
--      NAME    TYPE   SHUT    MODE    RATE   RATE  TARGET
        'inj1'   'WATER' 'OPEN' 'RATE'   5000/
/

TSTEP
0.5 1 2 4 8 16 32 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 36.5 
/

