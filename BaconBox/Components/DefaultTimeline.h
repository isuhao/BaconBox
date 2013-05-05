#ifndef BB_DEFAULT_TIMELINE_H
#define BB_DEFAULT_TIMELINE_H

#include "BaconBox/Components/Timeline.h"
#include "BaconBox/Components/MatrixComponent.h"

namespace BaconBox {
	class DefaultEntityContainer;
	class DefaultTimeline : public Timeline {
	public:
		DefaultTimeline();
		
		DefaultTimeline(const DefaultTimeline &src);
		
		virtual ~DefaultTimeline();
		
		DefaultTimeline &operator=(const DefaultTimeline &src);
		
		DefaultTimeline *clone() const;
		
		void receiveMessage(int senderID, int destID, int message, void *data);
		
		void update();
		
		void gotoAndPlay(int frame);
		
		void gotoAndStop(int frame);
		
		void nextFrame();
		
		void prevFrame();
		
		void play();
		
		void stop();
		
		int getCurrentFrame() const;
		bool isPlaying() const;
		
		int getNbFrames() const;
		
		void setNbFrames(int newNbFrames);
	private:
		 void setFrame(int frame);
		void gotoAndSetState(int frame, bool newPlaying);
		
		void initializeConnections();
		
		bool playing;
		
		int nbFrames;
		
		int currentFrame;
		DefaultEntityContainer * defaultEntityContainer;
		
		
	};
	
	
}

#endif /* defined(BB_DEFAULT_TIMELINE_H) */
