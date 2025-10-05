#include <string>

#include "GameInputContext.h"

class GameInputConfigReader {
   public:
    void read(const std::string& file_name, GameInputContext& context);

   private:
    Button getPhysicalButton(const std::string& name) const;
    Axis getPhysicalAxis(const std::string& name) const;
};