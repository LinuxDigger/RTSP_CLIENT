/*
 * EfficientAddOrUpdate.hpp
 *
 *  Created on: Aug 24, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_EFFICIENTADDORUPDATE_HPP_
#define INCLUDE_EFFICIENTADDORUPDATE_HPP_

#include "EfficientAddOrUpdate.h"

template<typename MapType, typename KeyArgType, typename ValueArgType>
typename MapType::iterator efficientAddOrUpdate(MapType &m, KeyArgType &k,
		ValueArgType &v) {
	typename MapType::iterator lb = m.lower_bound(k);
	if (lb != m.end() && !(m.key_comp()(k, lb->first))) {
		lb->second = v;
		return lb;
	} else {
		typedef typename MapType::value_type MVT;
		return m.insert(lb, MVT(k, v));
	}
}

#endif /* INCLUDE_EFFICIENTADDORUPDATE_HPP_ */
