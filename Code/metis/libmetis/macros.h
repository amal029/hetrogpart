/*
 * Copyright 1997, Regents of the University of Minnesota
 *
 * macros.h
 *
 * This file contains macros used in multilevel
 *
 * Started 9/25/94
 * George
 *
 * $Id: macros.h 10060 2011-06-02 18:56:30Z karypis $
 *
 */

#ifndef _LIBMETIS_MACROS_H_
#define _LIBMETIS_MACROS_H_

/* converts a user provided ufactor into a real ubfactor */
#define I2RUBFACTOR(ufactor) (1.0+0.001*(ufactor))


#endif
