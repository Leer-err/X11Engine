#ifndef TYPE_ID_HELPER_H
#define TYPE_ID_HELPER_H

#include "TypeId.h"

class TypeIdHelper {
   public:
    template <class Type>
    static const TypeId getTypeId() {
        static const TypeId TYPE_ID = type_count++;
        return TYPE_ID;
    }

    static const TypeId getTypesCount() { return type_count; }

   private:
    static TypeId type_count;
};

#endif  // TYPE_ID_HELPER_H
