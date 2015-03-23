#ifndef CMESH_H
#define CMESH_H
#include "CVector3f.h"
#include "CVertexBuffer.h"
#include "CIndexBuffer.h"
#include <vector>
enum EFillMode
{
    E_BACKFACING,
    E_FRONTFACING,
    E_NONE
};

class CMesh
{
    public:

        CMesh(int vertex_count = 8, int index_count = 36);

        ~CMesh();

        void create_arrays(int vertex_capacity, int index_capacity);

        void create_pentagon(const CVector3f &c, float radius);

        void destroy();

        void create_regular_polygon(const CVector3f &c, float radius, float pinch, unsigned int edge_count, EFillMode fill_mode);

        void create_prism(const CVector3f &c, float radius, float length, unsigned int segment_count, unsigned int slice_count);

		static void add_prism_element_count(unsigned int segment_count, unsigned int slice_count, unsigned int &vertex_count, unsigned int &triangle_count);

        void create_regular_polygon_indices(unsigned int base_vertex, unsigned int segment_count, EFillMode fill_mode);

        void create_cube(const CVector3f &a, const CVector3f &b);

        void create_indices(const CVector3f &a, const CVector3f &b);

        void create_quad(int a, int b, int c, int d);

		void calculate_normals();

		void add_polygon(const std::vector<size_t> &indices, bool flip);

		void add_polygon(const size_t *indices, size_t count, bool flip);

        void update(float dt);

        void draw();

		void fill_GPU_buffers();

		CVertexBuffer mVertexBuffer;
		CIndexBuffer<unsigned short>  mIndexBuffer;
        CVector3f *m_vertices;
        CVector3f *m_normals;
        CVector3f  m_position;
        CVector3f  m_orientation;
        CVector3f  m_scale;
        CVector3f  m_spin;
        unsigned int       *m_indices;
        unsigned int        m_vertex_count;
        unsigned int        m_index_count;
        unsigned int        m_index_capacity;
        unsigned int        m_vertex_capacity;
};

#endif
