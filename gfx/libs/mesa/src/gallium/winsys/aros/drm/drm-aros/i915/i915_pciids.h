/*
    Copyright 2009, The AROS Development Team. All rights reserved.
    $Id$
*/

#if !defined(I915_PCIIDS_H)
#define I915_PCIIDS_H

#include "drmP.h"

const struct drm_pciid i915_pciids [] =
{
    { 0x8086, 0x3577 },
    { 0x8086, 0x2562 },
    { 0x8086, 0x3582 },
    { 0x8086, 0x2572 },
    { 0x8086, 0x2582 },
    { 0x8086, 0x258a },
    { 0x8086, 0x2592 },
    { 0x8086, 0x2772 },
    { 0x8086, 0x27a2 },
    { 0x8086, 0x27ae },
    { 0x8086, 0x2972 },
    { 0x8086, 0x2982 },
    { 0x8086, 0x2992 },
    { 0x8086, 0x29a2 },
    { 0x8086, 0x29b2 },
    { 0x8086, 0x29c2 },
    { 0x8086, 0x29d2 },
    { 0x8086, 0x2a02 },
    { 0x8086, 0x2a12 },
    { 0x8086, 0x2a42 },
    { 0x8086, 0x2e02 },
    { 0x8086, 0x2e12 },
    { 0x8086, 0x2e22 },
    { 0x8086, 0x2e32 },
    { 0x8086, 0x2e42 },
    { 0x8086, 0xa001 },
    { 0x8086, 0xa011 },
    { 0x8086, 0x35e8 },
    { 0x8086, 0x0042 },
    { 0x8086, 0x0046 },

    { 0x0000, 0x0000 }
};

#endif /* I915_PCIIDS_H */
