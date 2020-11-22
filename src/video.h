#ifndef VIDEO_H_
#define VIDEO_H_

int video_init(int width, int height);

int video_scroll(int x, int y);

void video_update(int dt);

#endif	/* VIDEO_H_ */
