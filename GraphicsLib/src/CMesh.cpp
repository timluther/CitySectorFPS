#include "CMesh.h"
#include "CIndexBuffer.inl"
#include "CVector3f.h"
#include <cstddef>
#include <math.h>
#ifdef _MSC_VER
#include "windows.h"
#include "gl_includes.h"
#endif


#include "Utility.h"

#define PI 3.14159265358979f

CMesh::CMesh(int vertex_capacity, int index_capacity)
	:m_vertices(NULL)
    ,m_normals(NULL)
    ,m_indices(NULL)
    ,m_vertex_count(0)
    ,m_index_count(0)
    ,m_position(0, 0, 0)
    ,m_orientation(0, 0, 0)
    ,m_scale(1, 1, 1)
    ,m_spin(0, 0, 0)
	,mVertexBuffer(EVT_P_D4B_N_UV, 100, EVB_KEEP_BUFFER_FLAGS)
	,mIndexBuffer(100)
{

    create_arrays(vertex_capacity, index_capacity);
}

CMesh::~CMesh()
{
    destroy();
}

void CMesh::create_arrays(int vertex_capacity, int index_capacity)
{
    if (m_vertices != NULL)
    {
        if (vertex_capacity != m_vertex_capacity)
            delete [] m_vertices;
        else
            return;
    }
    m_vertex_capacity = vertex_capacity;

    if (vertex_capacity == 0)
        m_vertices = NULL;
    else
        m_vertices = new CVector3f[vertex_capacity];

    if (m_indices != NULL)
    {
        if (index_capacity != m_index_capacity)
            delete [] m_indices;
        else
            return;
    }
    m_index_capacity = index_capacity;

    if (index_capacity == 0)
        m_indices = NULL;
    else
        m_indices = new unsigned int[index_capacity];
}

void CMesh::create_pentagon(const CVector3f &c, float radius)
{
    for(int i = 0; i < 5; ++i)
    {
        float r = (float(i)/5.0f)*2.0f*PI;
        m_vertices[m_vertex_count++] = CVector3f(c.x + sinf(r)*radius, c.y + cosf(r)*radius, c.z);
    }

    m_indices[m_index_count++] = 3;
    m_indices[m_index_count++] = 1;
    m_indices[m_index_count++] = 4;

    m_indices[m_index_count++] = 1;
    m_indices[m_index_count++] = 0;
    m_indices[m_index_count++] = 4;

    m_indices[m_index_count++] = 3;
    m_indices[m_index_count++] = 2;
    m_indices[m_index_count++] = 1;
}

void CMesh::destroy()
{
    if(m_vertices != 0)
    {
        delete [] m_vertices;
        m_vertices = NULL;
    }

    if(m_indices != 0)
    {
        delete [] m_indices;
        m_indices = NULL;
    }
}

void CMesh::create_regular_polygon(const CVector3f &c, float radius, float pinch, unsigned int segment_count, EFillMode fill_mode)
{
    if((m_vertex_count + segment_count) >= m_vertex_capacity)
        throw COutOfBoundsException(__FILE__, __LINE__, 0, m_vertex_count + segment_count, m_vertex_capacity);
    float f_segment_count = float(segment_count);
    unsigned int base_vertex = m_vertex_count;
    for(unsigned int i = 0; i < segment_count; ++i)
    {
        float r = (float(i)/f_segment_count)*2.0f*PI;
        m_vertices[m_vertex_count++] = CVector3f(c.x + powf(sinf(r), pinch)*radius, c.y + powf(cosf(r), pinch)*radius, c.z);
    }
    create_regular_polygon_indices(base_vertex, segment_count, fill_mode);
}

float z_modifier(float dx, float radius)
{
    float cx = -PI * 0.5f;
    float dxp = dx * PI;
    return dx * radius;//(sin(cx + dxp) * radius);
}

float radius_modifier(float dx, float r)
{
    return powf(sin(dx * PI), 5.0f) * r;
}

void CMesh::create_prism(const CVector3f &c, float radius, float length, unsigned int segment_count, unsigned int slice_count)
{
    unsigned int base_vertex = m_vertex_count;
    float start_z = c.z + length*0.5f;
    float delta_z = length/slice_count;
    float current_z = start_z + delta_z;
    float pinch = 5;
    #ifdef OLD
	float zmod = z_modifier(0, radius);
    create_regular_polygon(CVector3f(c.x, c.y, zmod), radius_modifier(0,radius), pinch, segment_count, E_FRONTFACING);
    for(unsigned int i = 0; i < slice_count - 1; ++i)
    {
        float dz = (i + 1)/ (float) slice_count;
        float lradius = radius_modifier(dz, radius);
		float zmod = z_modifier(dz, radius);
		create_regular_polygon(CVector3f(c.x, c.y, zmod), lradius, pinch, segment_count, E_NONE);
        std::cout << "Z: " << dz << std::endl;
        current_z -= delta_z;
    }
    create_regular_polygon(CVector3f(c.x, c.y, z_modifier(1, radius)), radius_modifier(1, radius), pinch, segment_count, E_BACKFACING);
    #else

    for(unsigned int i = 0; i < slice_count ; ++i)
    {
        float dz = i/ (float) slice_count;
        float lradius = radius_modifier(dz, radius);
		float zmod = z_modifier(dz, radius);
		create_regular_polygon(CVector3f(c.x, c.y, zmod), lradius, pinch, segment_count, i == 0 ? E_FRONTFACING : i == slice_count ? E_BACKFACING : E_NONE);
        std::cout << "Z: " << dz << std::endl;
        current_z -= delta_z;
    }

    #endif
    int c_base = base_vertex;

    for(unsigned int slice_index = 0; slice_index < slice_count; ++slice_index)
    {
        for(unsigned int i = 0; i < segment_count -1; ++i)
        {
            int l_base = c_base + i;
            create_quad(l_base, l_base + segment_count, l_base + segment_count + 1, l_base + 1);
        }
        create_quad(c_base + segment_count - 1, c_base + segment_count*2 - 1, c_base + segment_count, c_base);
        c_base += segment_count;
    }
}

void CMesh::add_prism_element_count(unsigned int segment_count, unsigned int slice_count, unsigned int &vertex_count, unsigned int &triangle_count)
{
    vertex_count += segment_count*(slice_count + 1) + 1;
    triangle_count += ((segment_count - 2)*2); // this is for the end caps
    triangle_count += (segment_count * slice_count) * 2; //this is for the slices, or body of the prism
}

void CMesh::create_regular_polygon_indices(unsigned int base_vertex, unsigned int segment_count, EFillMode fill_mode)
{
    if (fill_mode != E_NONE)
    {
        unsigned int triangle_count = segment_count-2;
        unsigned int c = base_vertex + 1;
        if (fill_mode == E_FRONTFACING)
        {
            for(unsigned int i = 0; i < triangle_count; ++i)
            {
                m_indices[m_index_count++] = c+1;
                m_indices[m_index_count++] = c;
                m_indices[m_index_count++] = base_vertex;
                ++c;
            }
        }
        else
        {
            for(unsigned int i = 0; i < triangle_count; ++i)
            {
                m_indices[m_index_count++] = base_vertex;
                m_indices[m_index_count++] = c;
                m_indices[m_index_count++] = c+1;
                ++c;
            }
        }
    }
}

void CMesh::create_cube(const CVector3f &a, const CVector3f &b)
{
    create_arrays(8, 36);

    m_vertices[0] = CVector3f(a.x,a.y,a.z);
    m_vertices[1] = CVector3f(b.x,a.y,a.z);
    m_vertices[2] = CVector3f(b.x,b.y,a.z);
    m_vertices[3] = CVector3f(a.x,b.y,a.z);

    m_vertices[4] = CVector3f(a.x,a.y,b.z);
    m_vertices[5] = CVector3f(b.x,a.y,b.z);
    m_vertices[6] = CVector3f(b.x,b.y,b.z);
    m_vertices[7] = CVector3f(a.x,b.y,b.z);

    create_quad(0, 1, 2, 3);
    create_quad(4, 0, 3, 7);
    create_quad(5, 4, 7, 6);
    create_quad(1, 5, 6, 2);
    create_quad(3, 2, 6, 7);
    create_quad(4, 5, 1, 0);

    m_vertex_count = 8;
}

void CMesh::create_quad(int a, int b, int c, int d)
{
    ASSERT((m_index_count+4) < m_index_capacity);

    if((m_index_count+4) >= m_index_capacity)
        throw COutOfBoundsException(__FILE__, __LINE__, 0, m_index_count, m_index_capacity);
    m_indices[m_index_count++] = d;
    m_indices[m_index_count++] = b;
    m_indices[m_index_count++] = a;

    m_indices[m_index_count++] = d;
    m_indices[m_index_count++] = c;
    m_indices[m_index_count++] = b;
}

void CMesh::create_indices(const CVector3f &a, const CVector3f &b)
{
    m_indices[0] = 0;
    m_indices[1] = 1;
    m_indices[2] = 3;

    m_indices[3] = 1;
    m_indices[4] = 2;
    m_indices[5] = 3;

    m_indices[6] = 1;
    m_indices[7] = 4;
    m_indices[8] = 2;

    m_indices[9] = 4;
    m_indices[10] = 7;
    m_indices[11] = 2;

    m_indices[12] = 5;
    m_indices[13] = 0;
    m_indices[14] = 6;

    m_indices[15] = 0;
    m_indices[16] = 3;
    m_indices[17] = 6;

    m_indices[18] = 4;
    m_indices[19] = 5;
    m_indices[20] = 6;

    m_indices[21] = 5;
    m_indices[22] = 6;
    m_indices[23] = 7;

    m_indices[24] = 5;
    m_indices[25] = 4;
    m_indices[26] = 0;

    m_indices[27] = 4;
    m_indices[28] = 1;
    m_indices[29] = 0;

    m_indices[30] = 3;
    m_indices[31] = 2;
    m_indices[32] = 6;

    m_indices[33] = 2;
    m_indices[34] = 7;
    m_indices[35] = 6;

    m_index_count = 36;
}

void CMesh::update(float dt)
{
   m_orientation.x += m_spin.x * dt;
   m_orientation.y += m_spin.y * dt;
   m_orientation.z += m_spin.z * dt;
}
/*
&& (mVisible) && mMaterial
*/
void CMesh::draw()
{
#ifdef ANGLE

	if ((mIndexBuffer.Count() > 0))
	{

		CheckGLErrors("Error CCmpMesh::Draw ");
		

		//gShaderData.mModelMatrix = mWorldTransform;
		//gShaderData.BuildModelViewProjectionMatrix();

		//mMaterial->mVertexAttributes.Use(mVertices.GetGLHandle());

		//mMaterial->BeginDraw();
		// mIndexBuffer.GLIndexType()
		
		mIndexBuffer.Use();
		glDrawElements(GL_TRIANGLES, mIndexBuffer.Count(), GL_UNSIGNED_SHORT, 0);
		//mMaterial->EndDraw();
		
		
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

#else
	glPushMatrix();
    glScalef(m_scale.x, m_scale.y, m_scale.z);
    glTranslatef(m_position.x, m_position.y, m_position.z);
    glRotatef(m_orientation.x, 1, 0, 1);
    glRotatef(m_orientation.y, 1, 0, 1);
    glRotatef(m_orientation.z, 1, 0, 1);

    size_t tri_count = m_index_count / 3;

    size_t index_base = 0;
    glBegin(GL_TRIANGLES);
    for (size_t i = 0; i < tri_count; ++i)
    {
        const CVector3f &a = m_vertices[m_indices[index_base]];
        const CVector3f &b = m_vertices[m_indices[index_base + 1]];
        const CVector3f &c = m_vertices[m_indices[index_base + 2]];
        CVector3f ab (a.x - b.x, a.y - b.y, a.z - b.z);
        CVector3f ac (a.x - c.x, a.y - c.y, a.z - c.z);
        CVector3f normal = ab.normal().cross(ac.normal());
        glNormal3f(normal.x, normal.y, normal.z);
        glVertex3f(a.x, a.y, a.z);
        glVertex3f(b.x, b.y, b.z);
        glVertex3f(c.x, c.y, c.z);
        index_base += 3;
    }
    glEnd();
    glPopMatrix();
#endif
}

void CMesh::calculate_normals()
{

}

void CMesh::fill_GPU_buffers()
{
	mIndexBuffer.EnsureCapacity(m_index_count * 3);
	mVertexBuffer.EnsureCapacity(m_vertex_count);
	CVertexBuffer::VertexType *vertices = (CVertexBuffer::VertexType *)mVertexBuffer.Lock();
	unsigned short *indices = mIndexBuffer.Lock();
	size_t vsize = sizeof(vertices[0]);
	for (unsigned int i = 0; i < m_vertex_count; ++i)
	{
		vertices[i].mPosition = m_vertices[i];
		if (m_normals)
			vertices[i].mNormal	  = m_normals[i];
		vertices[i].mColour	  = 0xFFFFFFFF;
	}
		
	for (unsigned int i = 0; i < m_index_count; ++i)
	{
		indices[i] = m_indices[i];
	}
	
	mVertexBuffer.UnLock();
	mIndexBuffer.UnLock();
	mVertexBuffer.mCount = m_vertex_count;
	mIndexBuffer.mCount = m_index_count;
}