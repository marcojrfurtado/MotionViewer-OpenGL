#include "joint.hpp"
#include <numeric>
#include <cstring>
#include <cstdlib>

#include <GL/glew.h>
#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif


using namespace std;



bool Joint::process(FILE *fp) {	

	char next_str[100];

	// Sequential id used for the joints to identify their unique channel offset
	static int seq_id = 0;
	
	// Read identification for this joint
	fscanf(fp," %s {",next_str);


	//printf("%s\n",next_str);

	set_name(next_str);

	// Compute the channel offset here
	if ( this->parent && !is_end )  {
		this->channel_offset += 6 + (  3 * seq_id++ );
	}

	
	while ( fscanf(fp," %s",next_str) != EOF ) {

		if ( !strcmp(next_str,"JOINT" )  ) {
			Joint *child = new Joint(false,false,this);
			child->process(fp);
			this->subjoints.push_back(child);
		}	
		else if (  !strcmp(next_str,"End")  ) {
			Joint *child = new Joint(false,true,this);
			child->process(fp);
			this->subjoints.push_back(child);
		}	
		else if ( !strcmp(next_str,"OFFSET") ) {
			double x,y,z;
			fscanf(fp," %lf %lf %lf",&x,&y,&z);
			set_offset(x,y,z);
		}
		else if ( !strcmp(next_str,"CHANNELS") ) {

			read_channels(fp);

		}
		else if ( !strcmp(next_str,"}") )
			break;
		else 
			printf("%s\n",next_str);
	}

	return true;

}


void Joint::read_channels(FILE *fp) {


	int num_channels;
	fscanf(fp," %d",&num_channels);


	for(int i = 0; i < num_channels; i++ ) {

		char channel_type[50];
		fscanf(fp," %s",channel_type);

		if ( !strcmp(channel_type,"Xposition") ) 
			channels.push_back(X_POSITION);
		else if ( !strcmp(channel_type,"Yposition") )
			channels.push_back(Y_POSITION);
		else if ( !strcmp(channel_type,"Zposition") )
			channels.push_back(Z_POSITION);
		else if ( !strcmp(channel_type,"Xrotation") )
			channels.push_back(X_ROTATION);
		else if ( !strcmp(channel_type,"Yrotation") )
			channels.push_back(Y_ROTATION);
		else if ( !strcmp(channel_type,"Zrotation") )
			channels.push_back(Z_ROTATION);
		else {
			fprintf(stderr,"Error parsing channels.\n");
			exit(-1);
		}
	}
}

// USED ONLY FOR DEBUGGING
void Joint::pretty_print(const char *offset){


	char next_offset[100] = " \0";
	strcat(next_offset,offset);

	printf("%s %s\n",offset, this->get_name() );

	std::vector<Joint *>::iterator it;

	for( it = subjoints.begin(); it != subjoints.end(); it++ ) {
		(*it)->pretty_print(next_offset);
	}



}

void Joint::print(FILE *out_fp,const char *offset ) {

	char next_offset[15] = "\t\0";

	strcat(next_offset,offset);

	if ( is_root ) {
		fprintf(out_fp,"ROOT %s\n",this->get_name());
	} else if  ( is_end ) {
		fprintf(out_fp,"%sEnd Site\n",offset);
	} else {
		fprintf(out_fp,"%sJOINT %s\n",offset,this->get_name());
	}
	fprintf(out_fp,"%s{\n",offset);

	fprintf(out_fp,"%schannel offset: %d\n",offset,this->channel_offset);

	fprintf(out_fp,"%sOFFSET %.5lf %.5lf %.5lf\n",next_offset,original.x,original.y,original.z);

	if ( channels.size() > 0 ) {

		fprintf(out_fp,"%sCHANNELS %lu",next_offset,channels.size());
		vector<ChannelType>::iterator it;

		for(it=channels.begin(); it != channels.end(); it++ ) {

			switch(*it) {

				case X_POSITION:
					fprintf(out_fp," Xposition");
					break;
				case Y_POSITION:
					fprintf(out_fp," Yposition");
					break;
				case Z_POSITION:
					fprintf(out_fp," Zposition");
					break;
				case X_ROTATION:
					fprintf(out_fp," Xrotation");
					break;
				case Y_ROTATION:
					fprintf(out_fp," Yrotation");
					break;
				case Z_ROTATION:
					fprintf(out_fp," Zrotation");
					break;
			}
		}
		fprintf(out_fp,"\n");

	}

	vector<Joint *>::iterator itj;

	for(itj=subjoints.begin(); itj !=subjoints.end() ; itj++ ) {
		(*itj)->print(out_fp,next_offset);
	}
	fprintf(out_fp,"%s}\n",offset);

}

int Joint::count_hierarchy_channels() {
	int channel_count = 0;

	std::vector<Joint *>::iterator it;

	for( it = subjoints.begin(); it != subjoints.end() ; it++ ) 
		channel_count+=(*it)->count_hierarchy_channels();

	channel_count+=channels.size();

	return channel_count;
}

void Joint::render_transformation( const Motion::frame_data & data, bool original_pose ) {
	render_transformation(  data, data, 0 , original_pose );
}

void Joint::render_transformation( const Motion::frame_data & data, const Motion::frame_data & data2 , const float lambda, bool original_pose  ) {

	if ( this->channels.size() == 0 )
		return;
	
	glPushMatrix();

	if ( (this->is_root) && !(original_pose)  ) {
		// Linear interpolation between data and data2
		// If lambda == 0, we display only data 
		glTranslatef(((1-lambda)*data[0] ) + (lambda*data2[0]) ,((1-lambda)*data[1]) + (lambda*data2[1]),((1-lambda)*data[2] + (lambda*data2[2])));

	} else {
		glTranslatef(this->o.x,this->o.y,this->o.z);
	}

	

#ifndef JOINT_DISABLE_ROTATIONS

	if ( !original_pose ) {
	
		std::vector<ChannelType>::iterator it_channels;
		
		// Iterate on the channels
		int i = 0;

		// Identity quaternion
		Quaternion resQ1(1.0,0.0,0.0,0.0),resQ2(1.0,0.0,0.0,0.0);

		for( it_channels = channels.begin() ; it_channels != channels.end(); it_channels++ ) {
			
			double d1,d2;
			d1 = (PI/180.0)*data[this->channel_offset +i];
			d2 = (PI/180.0)*data2[this->channel_offset +i];

			if ( *it_channels == X_ROTATION ) {
	//			glRotatef(data[this->channel_offset + i ] ,1.0f,0.0f,0.0f);
				resQ1= resQ1 * Quaternion(cos(d1/2.0),sin(d1/2.0),0,0);
				resQ2= resQ2 * Quaternion(cos(d2/2.0),sin(d2/2.0),0,0);

			}
			else if ( *it_channels == Y_ROTATION ) {
				resQ1= resQ1 * Quaternion(cos(d1/2.0),0,sin(d1/2.0),0);
				resQ2= resQ2 * Quaternion(cos(d2/2.0),0,sin(d2/2.0),0);
			}
			else if ( *it_channels == Z_ROTATION ) {
				resQ1= resQ1 * Quaternion(cos(d1/2.0),0,0,sin(d1/2.0));
				resQ2= resQ2 * Quaternion(cos(d2/2.0),0,0,sin(d2/2.0));
			}

			i++;
		}


		// Interpolated quaternion

		// We interpolate everytime. If lambda == 0, only information from data is used anyway,
		Quaternion intQ;
		slerp(resQ1,resQ2,intQ,lambda);



		double rotAxis[4];
		//double mt[16];
		intQ.to_rotation_axis(rotAxis);
		//intQ.to_matrix(mt);
		glRotatef((180.0/PI)*rotAxis[0],rotAxis[1],rotAxis[2],rotAxis[3]);

	}


#endif

	std::vector< Joint * >::iterator it_sub;

	glm::vec3 origin(0.0,0.0,0.0);
	if ( subjoints.size() == 1 ) {
		render_bone(origin, (*subjoints.begin())->get_offset() );
	} else {

		glm::vec3 center = get_center();

		render_bone(origin,center);


		for( it_sub = subjoints.begin() ; it_sub != subjoints.end() ; it_sub++ ) {
			render_bone( center,  (*it_sub)->get_offset() );
		}
	}
	

	for( it_sub = subjoints.begin() ; it_sub != subjoints.end() ; it_sub++ ) {
		(*it_sub)->render_transformation(data,data2,lambda,original_pose);
	}
	
	glPopMatrix();

}

void Joint::render_bone(glm::vec3 p1, glm::vec3 p2) {
	
	glBegin(GL_LINES);
		glVertex3f(p1.x,p1.y,p1.z);
		glVertex3f(p2.x,p2.y,p2.z);
	glEnd();




}

void Joint::restore() {

	o.x = original.x;
	o.y = original.y;
	o.z = original.z;

	std::vector<Joint *>::iterator it;
	for( it = subjoints.begin() ; it != subjoints.end(); it++ ) {
		(*it)->restore();
	}
}

glm::vec3 Joint::get_center() {

	glm::vec3 ret(0.0,0.0,0.0);

	std::vector<Joint *>::iterator it;
	for( it = subjoints.begin() ; it != subjoints.end() ; it++ ) {
		glm::vec3 next = (*it)->get_offset();
		ret.x+= next.x;
		ret.y+= next.y;
		ret.z+= next.z;
	}

	ret.x /= subjoints.size() + 1;
	ret.y /= subjoints.size() + 1;
	ret.z /= subjoints.size() + 1;

	return ret;
}

void Joint::slerp(Quaternion q1, Quaternion q2, Quaternion &qr, double t)
{
   float w1, x1, y1, z1, w2, x2, y2, z2;
   Quaternion q2New;
   float theta, mult1, mult2;

   w1 = q1.w; x1 = q1.u.x; y1 = q1.u.y; z1 = q1.u.z; 
   w2 = q2.w; x2 = q2.u.x; y2 = q2.u.y; z2 = q2.u.z;
   
   // Reverse the sign of q2 if q1.q2 < 0.
   if (w1*w2 + x1*x2 + y1*y2 + z1*z2 < 0)  
   {
      w2 = -w2; x2 = -x2; y2 = -y2; z2 = -z2;
   }
	   
   theta = acos(w1*w2 + x1*x2 + y1*y2 + z1*z2);

   if (theta > 0.000001) 
   {
	  mult1 = sin( (1-t)*theta ) / sin( theta );
	  mult2 = sin( t*theta ) / sin( theta );
   }

   // To avoid division by 0 and by very small numbers the approximation of sin(angle)
   // by angle is used when theta is small (0.000001 is chosen arbitrarily).
   else
   {
      mult1 = 1 - t;
	  mult2 = t;
   }

   qr.w =  mult1*w1 + mult2*w2;
   qr.u.x =  mult1*x1 + mult2*x2;
   qr.u.y =  mult1*y1 + mult2*y2;
   qr.u.z =  mult1*z1 + mult2*z2;
   
}



