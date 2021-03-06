/*
 *  Player.cpp
 *  Homework4
 *
 *  Created by John Ryding on 4/23/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "Player.h"

#define FRAME_INC 10

Player::Player(Robot* _rbt): channels(45) {
	this->rbt = _rbt;
	this->currentFrame = 0;
	this->keyframeAmt = 0;
	this->isForward = true;
}

/*
 * Add a new keyframe to the animation
 */
void Player::addKeyFrame(){
	vector<float> limbRoation;
	int currentLimb = 0;
	
	for (int i = 0; i < channels.size(); i+=3) {
		limbRoation = this->rbt->getLimbRotation(currentLimb);
		
		Keyframe *frameX = new Keyframe();
		frameX->Time = this->keyframeAmt * FRAME_INC;
		frameX->Value = limbRoation[0];
		frameX->RuleIn = "linear";
		frameX->RuleOut = "linear";
		this->channels[i].insertKeyFrame(frameX);
		
		Keyframe *frameY = new Keyframe();
		frameY->Time = this->keyframeAmt * FRAME_INC;
		frameY->Value = limbRoation[1];
		frameY->RuleIn = "linear";
		frameY->RuleOut = "linear";
		this->channels[i+1].insertKeyFrame(frameY);
		
		Keyframe *frameZ = new Keyframe();
		frameZ->Time = this->keyframeAmt * FRAME_INC;
		frameZ->Value = limbRoation[2];
		frameZ->RuleIn = "linear";
		frameZ->RuleOut = "linear";
		this->channels[i+2].insertKeyFrame(frameZ);
		
		currentLimb++;
	}
	
	this->keyframeAmt++;
}

/*
 * Increment the animation by a single frame.
 */
void Player::incrementFrame(int *frameCnt){
	if (!channels[0].hasMinKeyFrames) { return; }
	
	if (this->currentFrame > this->channels[0].getMaxTime()) {
		this->currentFrame = 0;
	}
	
	bool isKeyframe = this->updatePose(true);
	*frameCnt = this->currentFrame;
	
	if (isKeyframe) {
		*frameCnt *= -1;
	}
	
	this->currentFrame++;
}

/*
 * Decrease the current animation by a single frame
 */
void Player::decrementFrame(int *frameCnt){
	if (!channels[0].hasMinKeyFrames) { return; }
	
	this->currentFrame--;
	
	if (this->currentFrame < 0) {
		this->currentFrame = this->channels[0].getMaxTime();
	}
	
	bool isKeyframe = this->updatePose(false);
	*frameCnt = this->currentFrame;
	
	if (isKeyframe) {
		*frameCnt *= -1;
	}
}

/*
 * Helper to update the pose of the robot .
 */
bool Player::updatePose(bool isForward){	
	float x=0.0, y=0.0, z=0.0;
	float direction = 1.0;
	
	if (!isForward) {
		direction = -1.0;
	}
	int curLimbIndex = 0;
	
	bool isKeyframe = false;
	for (int i = 0; i < channels.size(); i+=3) {
		x = channels[i].Evaluate(this->currentFrame);
		y = channels[i+1].Evaluate(this->currentFrame);
		z = channels[i+2].Evaluate(this->currentFrame);
		if (channels[i].isKeyFrame(this->currentFrame) ) {
			isKeyframe = true;
		} 
		
		this->rbt->setLimbRotation(curLimbIndex, x, y, z);
		curLimbIndex++;
	}
	
	return isKeyframe;
}

/*
 * Remove a keyframe from the animation
 */
void Player::removeKeyframe(int time){
	for (int i = 0; i < channels.size(); i++) {
		channels[i].removeKeyframe(time);
	}
	
	this->keyframeAmt--;
}

/*
 * Output the stirng representation of the animation
 */
string Player::toString(){
	ostringstream oss;
	
	for (int i = 0; i < channels.size(); i++) {
		oss << channels[i].toString();
	}
	
	return oss.str();
}

/*
 * Load an animation from the provided file
 */
void Player::loadAnim(char* fName){
	ifstream fs(fName);
	
	string line;
	if (fs.is_open()){
		int channelCount = 0;
		Channel *chan;
		
		while (fs.good()) {
			getline(fs, line);
			
			if (line == "channel"){
				chan = &(this->channels[channelCount]);
				channelCount++;
			} else if (line == "keyframe"){
				getline(fs, line);
				vector<string> kfVals = splitStringBySpace(line);
				
				Keyframe *frame = new Keyframe();
				frame->Time = convertToInt(kfVals[0]);
				frame->Value = convertToFloat(kfVals[1]);
				frame->RuleIn = kfVals[2];
				frame->RuleOut = kfVals[3];
				
				chan->insertKeyFrame(frame);
			}
		}
	}
}