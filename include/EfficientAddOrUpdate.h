/*
 * EfficientAddOrUpdate.h
 *
 *  Created on: Aug 24, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_EFFICIENTADDORUPDATE_H_
#define INCLUDE_EFFICIENTADDORUPDATE_H_

template<typename MapType, typename KeyArgType, typename ValueArgType>
typename MapType::iterator efficientAddOrUpdate(MapType &m, KeyArgType &k,
		ValueArgType &v);

#include "EfficientAddOrUpdate.hpp"
#endif /* INCLUDE_EFFICIENTADDORUPDATE_H_ */
