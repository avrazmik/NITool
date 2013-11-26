#include "stdafx.h"
#include "DepthModel.h"

/** 
 * Default constructor
 */
DepthModel::DepthModel()
{
	detected_targets = 0;
}

/** 
 * Destructor
 */
DepthModel::~DepthModel(void)
{
}

/** 
 * Method is used to align the depth and ir image pixels
 * CvPoint2D32f * Coorinates of the ir image pixel
 * @pararm int	The offset that should subtracted/added in x direction
 * @pararm int	The offset that should subtracted/added in y direction
 */
void DepthModel::subtractOffet(CvPoint2D32f *point, int offset_x, int offset_y)
{
	point->x = point->x + offset_x;
	point->y = point->y + offset_y;
}

/** 
 * Method is used to create a 3D depth lookup table
 * @param int	The width of table
 * @param int	The height of table
 * @param int	The depth of table
 * @return Lookup *	Pointer of the table
 */
Lookup * DepthModel::createLookup(int width, int height, int depth)
{
	lookup = new Lookup(width, height, depth);
	return lookup;
}

/** 
 * Method is used to get the pointer of the table
 * @return Lookup *	Pointer of the table
 */
Lookup * DepthModel::getLookup()
{
	return lookup;
}

/**
 * Method is used to save the lookup table into file
 * @param	Stream *	Pointer to the stream. Used to find the path of the folder
 */
void DepthModel::saveLookup(Stream *p)
{
	char dir[100];		
	sprintf(dir, CFG_STORAGE, p->_camera_id);
	strcat(dir, p->getStoragePrefix());
	strcat(dir, "depth.lookup");
	lookup->save(dir);
}

/**
 * Method is used to load the lookup table from file
 * @param	Stream *	Pointer to the stream. Used to find the path of the folder
 */
void DepthModel::loadLookup(Stream *p)
{
	char dir[100];
	sprintf(dir, CFG_STORAGE, p->_camera_id);
	strcat(dir, p->getStoragePrefix());
	strcat(dir, "depth.lookup");
	lookup->load(dir);
}

/**
 * Method is used to find chessboard corner positions in image cooridnate system 
 * @param CvMat *	The cooridnates of the checkerboard center
 * @param CvMat *	The rotation matrix of the checkerboard
 * @param CvPoint3D32f *	Corner positions
 * @return void
 */
void DepthModel::findChessboardCornerPositions(sFrameOfMocapData* current_frame, CvPoint3D32f *ret_positions)
{
	CvMat* target_center = cvCreateMat(1, 3, CV_32FC1);

	CV_MAT_ELEM(*target_center, float, 0, 0) = current_frame->RigidBodies[0].x * OptiTrack::getUnitConvertion();
	CV_MAT_ELEM(*target_center, float, 0, 1) = current_frame->RigidBodies[0].y * OptiTrack::getUnitConvertion() - (float) 945;
	CV_MAT_ELEM(*target_center, float, 0, 2) = current_frame->RigidBodies[0].z * OptiTrack::getUnitConvertion();

	cout<<current_frame->nRigidBodies<<endl;
	float	x = current_frame->RigidBodies[0].qx,
			y = current_frame->RigidBodies[0].qy,
			z = current_frame->RigidBodies[0].qz,
			w = current_frame->RigidBodies[0].qw;

	CvMat* target_rotation = cvCreateMat(3, 3, CV_32FC1);
	
	CV_MAT_ELEM(*target_rotation, float, 0, 0) = 1 - 2 * y * y - 2 * z * z;
	CV_MAT_ELEM(*target_rotation, float, 0, 1) = 2 * x * y - 2 * z * w;
	CV_MAT_ELEM(*target_rotation, float, 0, 2) = 2 * x * z + 2 * y * w;

	CV_MAT_ELEM(*target_rotation, float, 1, 0) = 2 * x * y + 2 * z * w;
	CV_MAT_ELEM(*target_rotation, float, 1, 1) = 1 - 2 * x * x - 2 * z * z;
	CV_MAT_ELEM(*target_rotation, float, 1, 2) = 2 * y * z - 2 * x * w;

	CV_MAT_ELEM(*target_rotation, float, 2, 0) = 2 * x * z - 2 * y * w;
	CV_MAT_ELEM(*target_rotation, float, 2, 1) = 2 * y * z + 2 * x * w;
	CV_MAT_ELEM(*target_rotation, float, 2, 2) = 1 - 2 * x * x - 2 * y * y;
	
	/*
	CV_MAT_ELEM(*target_rotation, float, 0, 0) = 1 - 2 * q2 * q2 - 2 * q3 * q3;
	CV_MAT_ELEM(*target_rotation, float, 0, 1) = 2 * q1 * q2 + 2 * q0 * q3;
	CV_MAT_ELEM(*target_rotation, float, 0, 2) = 2 * q1 * q3 - 2 * q0 * q2;

	CV_MAT_ELEM(*target_rotation, float, 1, 0) = 2 * q1 * q2 - 2 * q0 * q3;
	CV_MAT_ELEM(*target_rotation, float, 1, 1) = 1 - 2 * q1 * q1 - 2 * q3 * q3;
	CV_MAT_ELEM(*target_rotation, float, 1, 2) = 2 * q2 * q3 + 2 * q0 * q1;

	CV_MAT_ELEM(*target_rotation, float, 2, 0) = 2 * q1 * q3 + 2 *q0 * q2;
	CV_MAT_ELEM(*target_rotation, float, 2, 1) = 2 * q2 *q3 - 2 * q0 * q1;
	CV_MAT_ELEM(*target_rotation, float, 2, 2) = 1 - 2 * q1 * q1 - 2 * q2 * q2;
	*/

	int middle_w = (CFG_BOARD_W-1)/2;
	int middle_h = (CFG_BOARD_H-1)/2;

	CvMat* rotated_center = cvCreateMat(1, 3, CV_32FC1);
	for(int j = 0; j < rotated_center->cols; j++)
		CV_MAT_ELEM(*rotated_center, float, 0, j) = 0.0;
	
	for(int i = 0; i < target_rotation->cols; i++)
	{
		for(int j = 0; j < target_rotation->rows; j++)
		{
			CV_MAT_ELEM(*rotated_center, float, 0, i) += (-CV_MAT_ELEM(*target_rotation, float, i, j)) * CV_MAT_ELEM(*target_center, float, 0, j);
		}
	}
	
	// calculating the top left coordinates of the center
	CvPoint3D32f *center_tl_corner = new CvPoint3D32f;
	center_tl_corner->x = CV_MAT_ELEM(*rotated_center, float, 0, 0) - (CFG_BOARD_CELL_SIZE/2);
	center_tl_corner->y = CV_MAT_ELEM(*rotated_center, float, 0, 1) + (CFG_BOARD_CELL_SIZE/2);
	center_tl_corner->z = CV_MAT_ELEM(*rotated_center, float, 0, 2);
	
	CvPoint3D32f *side_tl_corner = new CvPoint3D32f;
	side_tl_corner->x = center_tl_corner->x + (-middle_w * CFG_BOARD_CELL_SIZE); 
	side_tl_corner->y = center_tl_corner->y + (middle_h * CFG_BOARD_CELL_SIZE);
	side_tl_corner->z = center_tl_corner->z;

	int current_index = 0;
	for(int i=0;i<CFG_BOARD_H; i++)
	{
		for(int j=0;j<CFG_BOARD_W; j++)
		{
			ret_positions[current_index].x = side_tl_corner->x + j * CFG_BOARD_CELL_SIZE;
			ret_positions[current_index].y = side_tl_corner->y - i * CFG_BOARD_CELL_SIZE;
			ret_positions[current_index].z = side_tl_corner->z;
			current_index++;
		}
		cout<<endl;
	}
	
	CvMat* rotation_tranpose = cvCreateMat(3, 3, CV_32FC1);
	cvTranspose(target_rotation, rotation_tranpose);

	CvMat* tmp = cvCreateMat(1, 3, CV_32FC1);
	CvMat* tmp_rotated = cvCreateMat(1, 3, CV_32FC1);
	
	for(int k=0; k < CFG_BOARD_H * CFG_BOARD_W; k++)
	{
		for(int j = 0; j < tmp_rotated->cols; j++)
			CV_MAT_ELEM(*tmp_rotated, float, 0, j) = 0.0;

		CV_MAT_ELEM(*tmp, float, 0, 0) = ret_positions[k].x;
		CV_MAT_ELEM(*tmp, float, 0, 1) = ret_positions[k].y;
		CV_MAT_ELEM(*tmp, float, 0, 2) = ret_positions[k].z;

		for(int i = 0; i < rotation_tranpose->cols; i++)
		{
			for(int j = 0; j < rotation_tranpose->rows; j++)
			{
				CV_MAT_ELEM(*tmp_rotated, float, 0, i) += (-CV_MAT_ELEM(*rotation_tranpose, float, i, j)) * CV_MAT_ELEM(*tmp, float, 0, j);
			}
		}

		ret_positions[k].x = CV_MAT_ELEM(*tmp_rotated, float, 0, 0);
		ret_positions[k].y =  CV_MAT_ELEM(*tmp_rotated, float, 0, 1);
		ret_positions[k].z =  CV_MAT_ELEM(*tmp_rotated, float, 0, 2);

	}
	
}