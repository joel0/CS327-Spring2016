//
// Created by joelm on 2016-04-06.
//

#include "item.h"
#include "item_descrip.h"

bool item::is_equipment() {
    return type < TYPE_SCROLL;
}