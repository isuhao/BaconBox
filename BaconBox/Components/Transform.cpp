#include "Transform.h"
#include "BaconBox/Console.h"
namespace BaconBox {
	 BB_ID_IMPL(Transform);
	
	Transform::Transform() : Component(), position(), rotation(0.0f), scale() {
		setName("Transform");
	}

	Transform::Transform(const Transform &src) : Component(), position(src.position), rotation(src.rotation), scale(src.scale) {
		setName("Transform");
	}

	Transform::~Transform() {
	}

	Transform &Transform::operator=(const Transform &src) {
		if (this != &src) {
			this->position = src.position;
			this->rotation = src.rotation;
			this->scale = src.scale;
		}

		return *this;
	}

	Transform *Transform::clone() const {
		return new Transform(*this);
	}
	
	void Transform::receiveMessage(int senderID, int destID, int message, void *data) {
            if(destID != Transform::ID)return;
			switch (message) {
			case Transform::MESSAGE_GET_POSITION:
				*reinterpret_cast<Vector2 *>(data) = this->getPosition();
				break;

			case Transform::MESSAGE_GET_ROTATION:
				*reinterpret_cast<float *>(data) = this->getRotation();
				break;

			case Transform::MESSAGE_GET_SCALE:
				*reinterpret_cast<Vector2 *>(data) = this->getScale();
				break;

			case Transform::MESSAGE_SET_POSITION:
				this->setPosition(*reinterpret_cast<Vector2 *>(data));
				break;

			case Transform::MESSAGE_SET_ROTATION:
				this->setRotation(*reinterpret_cast<float *>(data));
				break;

			case Transform::MESSAGE_SET_SCALE:
				this->setScale(*reinterpret_cast<Vector2 *>(data));
				break;

			default:
				break;
			}
	}
	

	const Vector2 &Transform::getPosition() const {
		return this->position;
	}

	void Transform::setPosition(const Vector2 &newPosition) {
		this->position = newPosition;
		sendMessage(Entity::BROADCAST, MESSAGE_POSITION_CHANGED, &(this->position));
	}

	float Transform::getRotation() const {
		return this->rotation;
	}

	void Transform::setRotation(float newRotation) {
		this->rotation = newRotation;
                sendMessage(Entity::BROADCAST, MESSAGE_ROTATION_CHANGED, &(this->rotation));
	}

	const Vector2 &Transform::getScale() const {
		return this->scale;
	}

	void Transform::setScale(const Vector2 &newScale) {
		this->scale = newScale;
                sendMessage(Entity::BROADCAST, MESSAGE_SCALE_CHANGED, &(this->scale));
	}
	
	
	TransformProxy::TransformProxy(Entity* entity, bool mustAddComponent): entity(entity){
	    if(mustAddComponent){
		transform= new Transform();
		entity->addComponent(transform);
	    }
	}
	    
	    
	const Vector2 &TransformProxy::getPosition() const{
	    return transform->getPosition();
	}

	void TransformProxy::setPosition(const Vector2 &newPosition){
	    transform->setPosition(newPosition);
	}

	float TransformProxy::getRotation() const{
	    return transform->getRotation();
	}

	void TransformProxy::setRotation(float newRotation){
	    transform->setRotation(newRotation);
	}

	const Vector2 &TransformProxy::getScale() const{
	    return transform->getScale();
	}

	void TransformProxy::setScale(const Vector2 &newScale){
	    transform->setScale(newScale);
	}
	
}