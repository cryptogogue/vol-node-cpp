// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_ACCESSORS_H
#define VOLITION_ACCESSORS_H

#include <volition/common.h>

namespace Volition {

#define GET_COMPOSED(type,name,member,fallback) \
	inline type get##name () const { return member ? member->get##name () : fallback; }

#define GET(type,name,member) \
	inline type get##name () const { return member; }

#define SET(type,name,member) \
	inline void set##name ( type value ) { member = value; }

#define HAS(name,member,value) \
	inline bool has##name () const { return member == value; }

#define IS(name,member,value) \
	inline bool is##name () const { return member == value; }

#define GET_SET(type,name,member) \
	inline type get##name () const { return member; } \
	inline void set##name ( type value ) { member = value; }

} // namespace Volition
#endif
