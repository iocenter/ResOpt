# bonmin libraries:
LIBS += -L$$PWD/../Bonmin-stable/build/lib -lbonmin -lipopt -lCbc -lCgl -lOsiClp -lOsi -lClp -lCoinUtils

# ipopt libraries (from ipopt_addlibs_cpp.txt):
LIBS += -L$$PWD/../bonmin/lib \
    -lipopt -ldl -lcoinmumps -lpthread -lcoinlapack -lcoinblas -lgfortran -lm -lgcc_s -lquadmath

# cgl libraries (from cgl_addlibs.txt):
LIBS += -L$$PWD/../Bonmin-stable/build/lib \
    -lCgl -lOsiClp -lClp -lOsi -lcoinmumps -lpthread -lCoinUtils -lcoinlapack -lcoinblas -lgfortran -lm -lgcc_s -lquadmath

# osi libraries (from osi_addlibs.txt):
LIBS += -L$$PWD/../Bonmin-stable/build/lib \
    -lOsi -lCoinUtils -lcoinlapack -lcoinblas -lgfortran -lm -lgcc_s -lquadmath

# clp libraries (from clp_addlibs.txt):
LIBS += -L$$PWD/../Bonmin-stable/build/lib \
    -lClp -lcoinmumps -lpthread -lCoinUtils -lcoinlapack -lcoinblas -lgfortran -lm -lgcc_s -lquadmath

# coinutils libraries (from coinutils_addlibs.txt):
LIBS += -L$$PWD/../Bonmin-stable/build/lib \
    -lCoinUtils -lcoinlapack -lcoinblas -lgfortran -lm -lgcc_s -lquadmath

# NOMAD library
LIBS += $$PWD/../nomad/lib/nomad.a

# bonmin header files
INCLUDEPATH += $$PWD/../Bonmin-stable/build/include/coin


#NOMAD header files
INCLUDEPATH += $$PWD/../nomad/src
