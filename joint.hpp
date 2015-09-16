#ifndef JOINT_H
#define JOINT_H


#include <vector>
#include <string>
#include <cstdio>
#include <glm/glm.hpp>
#include "motion.hpp"

// Used for our spherical linear interpolation
#include "quaternion.hpp"


// Uncomment this to disable rotations ( debugging )
//#define JOINT_DISABLE_ROTATIONS

#define DISABLE_SLERP 0

class Joint {

	public:
		Joint(bool root = false, bool end = false, Joint *parent=0) {
			is_root=root;
			is_end=end;
			this->o.x = this->o.y = this->o.z = 0;
			this->original.x = this->original.y = this->original.z = 0;
			this->parent = parent;
			this->initialized = false;
			this->channel_offset = 0;
		}

		// Reads a joint from the FILE fp.
		// As joint descriptions are recursive, this is used to process the joint-to-joint connections and offsets for
		// the sampled motion data. 
		//Starts reading after a joint keyword ( ROOT, JOINT or End)
		// FILE fp must have previously opened.
		// In case of failure in the parsing process, returns false.
		bool process(FILE *fp);

		// class setters
		// Sets the name of this joint
		void set_name(const char *new_name) { name = new_name;  }
		void set_offset(const double x, const double y, const double z) { 
			this->o.x = x;
			this->o.y = y;
			this->o.z = z;

			if ( !initialized ) {
				this->original.x = x;
				this->original.y = y;
				this->original.z = z;
				this->initialized = true;
			}
		}

		const char * get_name() { return name.c_str(); }

		// Writes the joint, and its hierachy, to the file *out_fp
		void print(FILE *out_fp,const char *offset = "\0" );

		// Debugging print
		void pretty_print(const char *offset);

		const std::vector<Joint *> & get_children() { return subjoints;   }
		bool has_children() { return subjoints.size() > 0 ;  }

		const Joint * get_parent() { return parent;  }

		const glm::vec3 get_offset() { return o; }

		int count_hierarchy_channels();

		// Based on motion information, run the transformations on the hierarchy
		void render_transformation( const Motion::frame_data & data , bool original_pose=false );
		

		// Render this joint and all its subjoints using frames data and data2 ( interpolated )
		void render_transformation( const Motion::frame_data & data, const Motion::frame_data & data2, const float lambda , bool original_pose=false );

		// Returns all offsets to their original position
		// Recursive functions, restores all the hierarchy
		void restore();


		void translate( double x, double y, double z ) {
			o.x+=x;
			o.y+=y;
			o.z+=z;
		}

	private:

		// OFFSET structure
		glm::vec3 o;
		glm::vec3 original;
		bool initialized;

		// CHANNEL TYPES
		typedef enum {
			X_POSITION = 0,
			Y_POSITION,
			Z_POSITION,
			X_ROTATION,
			Y_ROTATION,
			Z_ROTATION
		} ChannelType;

		// ChannelVector
		std::vector<ChannelType> channels;
		int channel_offset;

		// Read channels description from the file
		void read_channels(FILE *fp);

		Joint *parent;

		
		std::vector<Joint *> subjoints;
		
		// Name of this joint
		std::string name;

		// The root and the end elementsin the hierachy are also considered joints in this
		// implementation.
		bool is_root;
		bool is_end;

		//HELPER functions

		glm::vec3 get_center();

		// Render a a line between p1 and p2
		void render_bone(glm::vec3 p1, glm::vec3 p2); 
		
		
		// Spherical interpolation, used for rotations
		void slerp(Quaternion q1, Quaternion q2, Quaternion &qr , double lambda);

};



#endif
