#ifndef BIT_UTIL_H
#define BIT_UTIL_H

#define _BV(n) (1u << n)

#define set(addr, bitpos) (addr |= _BV(bitpos))
#define get(addr, bitpos) (addr & _BV(bitpos))
#define reset(addr, bitpos) (addr &= ~_BV(bitpos))

#endif /* BIT_UTIL_H */
