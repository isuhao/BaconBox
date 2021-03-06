#ifndef BB_COMPONENT_H
#define BB_COMPONENT_H

#include <string>
#include <vector>
#include "BaconBox/Core/IDManager.h"

#include <sigly.h>


//To use this macro, your Proxy must take these two parameters NAMES ARE IMPORTANT (Entity* entity, bool mustAddComponent)
#define BB_PROXY_CONSTRUCTOR(componentConstructor) ComponentProxy(entity, (mustAddComponent ? componentConstructor : NULL))



namespace BaconBox {
	class Entity;
	class IComponentConnection;

	class Component : public sigly::HasSlots<SIGLY_DEFAULT_MT_POLICY> {
		friend class Entity;
	public:
		BB_ID_HEADER;

		Component();

		Component(const Component &src);

		virtual ~Component();

		Component &operator=(const Component &src);

		virtual Component *clone() const;

		void sendMessage(int destID, int message, void *data);
		virtual void receiveMessage(int senderID, int destID, int message, void *data);

		virtual void update();

		const std::string &getComponentName() const;

		template <typename T>
		T *getEntity() const {
			return reinterpret_cast<T *>(this->entity);
		}

		Entity *getEntity() const ;


	protected:
		void refreshConnections();

		void addConnection(IComponentConnection *newConnection);
		virtual void setEntity(Entity *newEntity);

	private:

		Entity *entity;

		std::vector<IComponentConnection *> connections;
	};


	class ComponentProxy {
	protected:
		Entity *entity;

		ComponentProxy(Entity *entity, Component *newComponent);
		Component *component;
	};
}

#endif
