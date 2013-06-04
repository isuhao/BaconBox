
%module BaconBox

#if defined(BB_LUA)
	%native(cast) int luacast(lua_State*L);

%luacode {
  function class(base, init)
   local c = {}    -- a new class instance
   if not init and type(base) == 'function' then
      init = base
      base = nil
   elseif type(base) == 'table' then
    -- our new class is a shallow copy of the base class!
      for i,v in pairs(base) do
         c[i] = v
      end
      c._base = base
   end
   -- the class will be the metatable for all its objects,
   -- and they will look up their methods in it.
   c.__index = c

   -- expose a constructor which can be called by <classname>(<args>)
   local mt = {}
   mt.__call = function(class_tbl, ...)
   local obj = {}
   setmetatable(obj,c)
   if class_tbl.init then
      class_tbl.init(obj,...)
   else 
      -- make sure that any stuff from the base class is initialized!
      if base and base.init then
      base.init(obj, ...)
      end
   end
   return obj
   end
   c.init = init
   c.is_a = function(self, klass)
      local m = getmetatable(self)
      while m do 
         if m == klass then return true end
         m = m._base
      end
      return false
   end
   setmetatable(c, mt)
   return c
end

LuaEntity = class()
function LuaEntity:init(entity)
  self.entity = entity
  self.entity:setLuaClass(self)
  for key, value in pairs(getmetatable(self.entity)[".fn"]) do
      if self[key] == nil  and key ~=  "update" then
        self[key] = function(self, ...) return self.entity[key](self.entity, ...);end
      end
  end
end



LuaState = class(LuaEntity)
function LuaState:init(name)
  LuaEntity.init(self, BaconBox.State(name))
  self.state = self.entity
end
}
#endif


%{

  #include "BaconBox/Helper/SafeEnum.h"
  #include "BaconBox/Platform.h"

	#include "BaconBox/PlatformFlagger.h"
	#include "BaconBox/Core/IDManager.h"
  #ifdef BB_FLASH_PLATFORM
	#include <AS3/AS3.h>
	#include <AS3/AS3++.h>
	#include "BaconBox/Components/Flash/MovieClipHolder.h" 
	#endif //BB_FLASH_PLATFORM


  #include "BaconBox/Display/Window/WindowOrientation.h"
  #include "BaconBox/Display/Window/MainWindow.h"

  #include "BaconBox/Input/InputState.h"
  #include "BaconBox/Input/InputSignalData.h"
  #include "BaconBox/Input/Pointer/CursorButton.h"


  #include "BaconBox/SignalSlots/SignalData.h"

  #include "BaconBox/Input/Keyboard/Key.h"
  #include "BaconBox/Input/Keyboard/Keyboard.h"
  #include "BaconBox/Input/Keyboard/KeyboardState.h"

  #include "BaconBox/Input/Keyboard/KeySignalData.h"
  #include "BaconBox/Input/Keyboard/KeyMaskSignalData.h"

  #include "BaconBox/Input/Pointer/CursorState.h"
  #include "BaconBox/Input/Pointer/PointerState.h"
  #include "BaconBox/Input/Pointer/PointerSignalData.h"
  #include "BaconBox/Input/Pointer/PointerButtonSignalData.h"

	#include "BaconBox/Core/Component.h"
	#include "BaconBox/Core/Entity.h"
  #include "BaconBox/Display/Color.h"
  #include "BaconBox/ColorMatrix.h" 
  #include "BaconBox/Components/ColorTransform.h"
  #include "BaconBox/Components/HasName.h"

	#include "BaconBox/Vector2.h"
	#include "BaconBox/Components/Transform.h"
  #include "BaconBox/Display/ColorFormat.h"
  #include "BaconBox/Display/TextureInformation.h"


  #include "BaconBox/Display/SubTextureInfo.h"
	#include "BaconBox/MovieClipEntity/MovieClipEntity.h"
	#include "BaconBox/EntityFactory.h"
  #include "BaconBox/Components/HasName.h"
  #include "BaconBox/Display/Camera.h"
  #include "BaconBox/Core/State.h"
  #include "BaconBox/Core/Engine.h"
  #include "BaconBox/Display/Text/FontFormat.h"
  #include "BaconBox/Components/Timeline.h"
  #include "BaconBox/Components/EntityContainer.h"

  #include "BaconBox/Display/Text/Font.h"
  #include "BaconBox/Helper/ResourcePathHandler.h"
  #include "BaconBox/ResourceManager.h"
#include "BaconBox/Display/Text/TextAlignment.h"
#include "BaconBox/Components/TextComponent.h"
#include "BaconBox/Display/Text/TextEntity.h"
#include "BaconBox/AxisAlignedBoundingBox.h"
  #include "BaconBox/Components/AABBHitBox.h"

  #include "BaconBox/Components/SizeComponent.h"


#include "BaconBox/Symbol.h"

#if defined(BB_LUA)
  #include "BaconBox/Components/Lua/LuaEntity.h"
#endif

namespace BaconBox{
class BaseEngine;
#ifdef BB_FLASH_PLATFORM
class FlashEngine;
#endif
}

///////////////////////
// Helper functions
///////////////////////
#if defined(BB_LUA)
	swig_type_info * getTypeByName(lua_State* L, const char * name){
		swig_module_info* module=SWIG_GetModule(L);
		for (int i =0; i < module->size; i++){
			swig_lua_class* luaclass = (swig_lua_class*) module->types[i]->clientdata;
			if(luaclass&& strcmp(luaclass->name, name) ==0){
				return module->types[i];
			}
		}
		return NULL;
	}

  int luaUserDataconvertPtr(lua_State* L,int index,void** ptr,swig_type_info *type,int flags){
    return SWIG_Lua_ConvertPtr(L,index,ptr,type,flags);
  }

  void pushLuaWrapperBySwigType(lua_State* L,void* ptr,swig_type_info *type,int own){
    SWIG_NewPointerObj(L, ptr, type, own);
  }

	int luacast(lua_State*L){
		void * myDataPtr;
        void ** ptrToPtr = &myDataPtr;

        const char * type_name = luaL_checkstring(L, 2);
        swig_type_info * type = getTypeByName(L, type_name);

        SWIG_ConvertPtr(L, 1, ptrToPtr, NULL, 0);


        // Search the SWIG module for a swig_type_info that contains the data
        // type string that was passed as the second parameter

        if(type == NULL)
        {

                lua_pushnumber(L, -2);
                lua_pushstring(L,"Failed to find swig_type_info for specified type.");
                return 2;
        }

        // Using the swig_type_info that we found, create a new object on 
        // the stack of the desired data type and return.
        SWIG_Lua_NewPointerObj(L, myDataPtr, type, 0);

        return 1;
	}
	#endif

class SoundInfo;
#include "BaconBox/Audio/AudioState.h"
#include "BaconBox/Audio/AudioEngine.h"
#include "BaconBox/Audio/SoundEngine.h"
#include "BaconBox/Audio/Sound.h"

#include "BaconBox/Audio/SoundFX.h"

#include "BaconBox/Audio/MusicEngine.h"
#include "BaconBox/Audio/BackgroundMusic.h"

%}
namespace BaconBox{
class BaseEngine;

#ifdef BB_FLASH_PLATFORM
class FlashEngine;
#endif

}

%include "BaconBox/Core/IDManager.h"

%ignore onPointerMove(PointerButtonSignalData);




#if defined(BB_LUA)
		%include "std_string.i"


%typemap(arginit) SWIGTYPE *self %{
  swig_type_info * selfType = $descriptor;
    SWIG_ConvertPtr(L,1,(void**)&$1,selfType,0);
%}


   %typemap(typecheck) lua_State* {
      $1 = lua_istable(L,$input);
    }

  %typemap(arginit) lua_State* %{
    #ifdef IMPOSSIBLE_MACRO_JUST_TO_SKIP_SWIG_ARG_COUNT_CHECK_I_SHOULD_REALLY_FIND_A_FIX_IN_THE_NEAR_FUTURE
    %}
    
 

    %typemap(in) (lua_State*) %{ 
   #endif //IMPOSSIBLE_MACRO_JUST_TO_SKIP_SWIG_ARG_COUNT_CHECK_I_SHOULD_REALLY_FIND_A_FIX_IN_THE_NEAR_FUTURE
        
    $1 = L;
    %}



#endif






#ifdef BB_AS3_SWIG


%typemap(astype) (AS3::local::var) "*";

%typemap(in) (AS3::local::var aMC) {
	AS3_GetVarxxFromVar($1, $input);
}



%rename("getComponentByID") getComponent(int) const;
%rename("getComponentByName") getComponent(const std::string &) const;


%ignore Component(const Component &src);
%ignore Entity(const Entity &src);
%ignore Transform(const Transform &src);


#endif

%include "BaconBox/Helper/SafeEnum.h"



%include "BaconBox/Core/Component.h"

%include "BaconBox/Components/HasName.h"


%include "BaconBox/Core/Entity.h"
%include "BaconBox/Display/Color.h"
%include "BaconBox/ColorMatrix.h" 
%include "BaconBox/Components/ColorTransform.h" 

//%include "BaconBox/Vector2.h"

namespace BaconBox{
	class Vector2{
		public:
		Vector2();
		#if defined(BB_LUA)
		Vector2(float x, float y);
		#endif

    Vector2 operator+(const Vector2 &other) const;
    Vector2 operator+(float delta) const;

    Vector2 operator-(const Vector2 &other) const;
    Vector2 operator-(float delta) const;


    float operator*(const Vector2 &other) const;
    Vector2 operator*(float delta) const;

    Vector2 operator/(float delta) const;


  
    float getLength() const;
    void setLength(float newLength);
    float getSquaredLength() const;
    float getDotProduct(const Vector2 &other) const;
    Vector2 getNormalized() const;
    void normalize();
    Vector2 &coordinatesMultiply(const Vector2 &other);
    Vector2 getCoordinatesMultiplication(const Vector2 &other) const;
    Vector2 &coordinatesDivide(const Vector2 &other);
    Vector2 getCoordinatesDivision(const Vector2 &other) const;
    float getAngle() const;
    float getAngleBetween(const Vector2 &other) const;
    void setAngle(float newAngle);
    Vector2 getRotated(float angle) const;
    void rotate(float angle);
    Vector2 getProjection(const Vector2 &direction) const;
    void project(const Vector2 &direction);
    Vector2 getReflected(const Vector2 &mirror) const;
    void reflect(const Vector2 &mirror);


		float x;
		float y;
	};

}


%include "BaconBox/PlatformFlagger.h"
%include "BaconBox/Platform.h"

%include "BaconBox/Input/InputState.h"
%include "BaconBox/SignalSlots/SignalData.h"

%include "BaconBox/Display/Window/WindowOrientation.h"
%include "BaconBox/Display/Window/MainWindow.h"

%include "BaconBox/Input/Keyboard/Key.h"
%include "BaconBox/Input/Keyboard/KeyboardState.h"
%include "BaconBox/Input/Keyboard/KeySignalData.h"
%include "BaconBox/Input/Keyboard/KeyMaskSignalData.h"
%include "BaconBox/Input/Keyboard/Keyboard.h"

%include "BaconBox/Input/Pointer/CursorButton.h"

%include "BaconBox/Input/InputSignalData.h"
%include "BaconBox/Input/Pointer/CursorState.h"
%include "BaconBox/Input/Pointer/PointerState.h"
%include "BaconBox/Input/Pointer/PointerSignalData.h"
%include "BaconBox/Input/Pointer/PointerButtonSignalData.h"

%include "BaconBox/Components/Transform.h"
%include "BaconBox/AxisAlignedBoundingBox.h"
 %include "BaconBox/Components/AABBHitbox.h"
  %include "BaconBox/Components/SizeComponent.h"

%include "BaconBox/Display/ColorFormat.h"


%include "BaconBox/Display/TextureInformation.h"
%include "BaconBox/Display/SubTextureInfo.h"


%include "BaconBox/Components/Timeline.h"
%include "BaconBox/Components/EntityContainer.h"



#if defined(BB_LUA)
%include "BaconBox/Components/Lua/LuaEntity.h"
#endif
%include "BaconBox/MovieClipEntity/MovieClipEntity.h"
%include "BaconBox/Display/Camera.h"

%include "BaconBox/Core/State.h"

%include "BaconBox/Core/Engine.h"

%include "BaconBox/Audio/AudioState.h"
%include "BaconBox/Audio/Sound.h"

%include "BaconBox/Audio/AudioEngine.h"
%include "BaconBox/Audio/SoundEngine.h"
%include "BaconBox/Audio/SoundFX.h"

%include "BaconBox/Audio/MusicEngine.h"
%include "BaconBox/Audio/BackgroundMusic.h"
%include "BaconBox/Display/Text/FontFormat.h"

%include "BaconBox/Display/Text/Font.h"

%include "BaconBox/Display/Text/TextAlignment.h"
%include "BaconBox/Components/TextComponent.h"
%include "BaconBox/Display/Text/TextEntity.h"


%ignore getMovieClipEntityFromSymbol;
%include "BaconBox/EntityFactory.h" 


%include "BaconBox/Helper/ResourcePathHandler.h"

namespace BaconBox{


  class ResourceManager{
    public:

    static SoundInfo *loadSound(const std::string &key,
                                const std::string &filePath,
                                bool overwrite = false);

    static SoundInfo *loadSoundRelativePath(const std::string &key,
                const std::string &relativePath,
                bool overwrite = false);
              

    static SoundInfo *loadSoundFromBundle(const std::string &key,
                                const std::string &bundleKey,
                                bool overwrite = false);

    static MusicInfo *loadMusicFromBundle(const std::string &key,
                                const std::string &bundleKey,
                                bool overwrite = false);          
                                

     static MusicInfo *loadMusic(const std::string &key,
                                const std::string &filePath,
                                bool overwrite = false);
                                

     static MusicInfo *loadMusicRelativePath(const std::string &key,
                                            const std::string &relativePath,
                                            bool overwrite = false);       

  static void loadFlashExporterXML(const std::string & xmlPath, const std::string & secondXMLPath = "");

    static SoundInfo *getSound(const std::string &key);

    static MusicInfo *getMusic(const std::string &key);


static Font *loadFont(const std::string &key,
                          const std::string &path, bool overwrite = false);

static Font *loadFontRelativePath(const std::string &key,
                                      const std::string &relativePath,
                                      bool overwrite = false);


    static Font *getFont(const std::string &key);

    static void removeFont(const std::string &key);

        static void removeTexture(const std::string &key);

    static void removeSound(const std::string &key);

    static void removeMusic(const std::string &key);
  };


}

