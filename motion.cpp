#include "motion.hpp"

bool Motion::process( FILE *fp ) {

	fscanf(fp," Frames: %d",&this->frames);
	fscanf(fp," Frame Time: %lf",&this->frametime);

	// Set frame size
	frame_set.resize(this->frames);


	for(int i = 0; i < this->frames; i++ ) {

		double next_double;

		double position[3];


		for( int j = 0; j < this->frame_data_size; j++ ) {	
			fscanf(fp," %lf",&next_double);
			frame_set[i].push_back(next_double);

			if ( j < 3 )
				position[j] = next_double;
		}
		update_boundaries(position);
		add_mean_vertex(position);
		
	}
	compute_mean_vertex();



	return true;

}


void Motion::print( FILE *out_fp ) {


	fprintf(out_fp,"Frames: %d\n",this->frames);
	fprintf(out_fp,"Frame Time: %.7lf\n",this->frametime);

	std::vector< frame_data >::iterator it_frameset;

	for( it_frameset = frame_set.begin() ; it_frameset != frame_set.end() ; it_frameset++ ) {

		frame_data::iterator it_frame;

		for( it_frame = (*it_frameset).begin() ; it_frame != (*it_frameset).end() ; it_frame++ ) {
			fprintf(out_fp,"%.4lf ",*it_frame);
		}
		fprintf(out_fp,"\n");
	}

}

void Motion::update_boundaries(double *pos) {

	max.x = pos[0] > max.x? pos[0] : max.x;
	max.y = pos[1] > max.y? pos[1] : max.y;
	max.z = pos[2] > max.z? pos[2] : max.z;
	
	min.x = pos[0] < min.x? pos[0] : min.x;
	min.y = pos[1] < min.y? pos[1] : min.y;
	min.z = pos[2] < min.z? pos[2] : min.z;

}
