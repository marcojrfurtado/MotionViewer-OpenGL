#ifndef MOTION_H
 #define MOTION_H

#include <vector>
#include <cstdio>
#include <glm/glm.hpp>


class Motion {
	public:
		// Type definition
		typedef std::vector< double > frame_data;
		
		
		Motion() {
			frames= 0;
			frametime= 0;
			frame_data_size=0;
			min =max = mean = glm::vec3(0,0,0);
			mean_count=0;
		}
		// Reas the motion data from FILE fp
		bool process(FILE *fp);

		void set_frame_data_size(const int n) { this->frame_data_size = n; }

		// Returns the frame rate in milliseconds
		int get_frame_rate() { return ( int ) frametime*1000; }

		void set_frame_rate( int time_ms ) {  frametime = time_ms / 1000;  }


		// Get motion information
		const std::vector< frame_data > & get_frame_set() { return frame_set; }

		// Print motion information to FILE out_fp
		void print(FILE *out_fp);

		// Retrieve information from the frame for the camera placement
		glm::vec3 get_max() { return max; }
		glm::vec3 get_min() { return min; }
		glm::vec3 get_mean() { return mean; }

	private:

		// Used during preprocess do determine camera position
		glm::vec3 max, min, mean;
		int mean_count;

		int frames;

		// frame time in BVH format
		double frametime;
		int frame_data_size;

		std::vector< frame_data > frame_set;

		void update_boundaries(double *pos);

		void add_mean_vertex(double *pos) {
			mean_count++;

			mean.x+=pos[0];
			mean.y+=pos[1];
			mean.z+=pos[2];
		}
		void compute_mean_vertex() {
			mean.x/=mean_count;
			mean.y/=mean_count;
			mean.z/=mean_count;
		}
		
};

#endif
