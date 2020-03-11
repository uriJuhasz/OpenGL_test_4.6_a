#include "Mesh.h"

#include "Utilities/Misc.h"

#include <vector>
#include <array>
#include <algorithm>
#include <cassert>
#include <iostream>
#include <chrono>
using namespace std;

class IntRange final
{
public:
	IntRange(const int min, const int max) : m_range{min, max} {}

	int operator[](const int i) const { return m_range[i]; }
	array<int,2> m_range;

	class Iter final
	{
		friend class IntRange;
		Iter(const int i) : m_i(i) {}
	public:
		Iter& operator++() { m_i++; return *this; }
		int operator*() const { return m_i; }
	private:
		int m_i;
	};
	Iter begin() const { return Iter(m_range[0]); }
	Iter end() const { return Iter(m_range[1]); }
};
bool operator==(const IntRange::Iter& i0, const IntRange::Iter& i1)
{
	return *i0 == *i1;
}
bool operator!=(const IntRange::Iter& i0, const IntRange::Iter& i1)
{
	return !(i0 == i1);
}

inline int p1m3(const int i)
{
	return (i == 0) ? 1 : ((i == 1) ? 2 : 0);
}
void calculateTopology1(Mesh& mesh)
{
	const auto& faces = mesh.m_faces;
	const auto numVertices = mesh.numVertices();
	const int numFaces = mesh.numFaces();

	auto& faceEdges = mesh.m_faceEdges;
	auto& edges = mesh.m_edges;
	auto& allVertexEdgeLists = mesh.m_allVertexEdgeLists;
	auto& vertexEdgeIndicesRanges = mesh.m_vertexEdgeIndicesRanges;

	faceEdges.clear();
	edges.clear();
	allVertexEdgeLists.clear();
	vertexEdgeIndicesRanges.clear();


	cout << " calculate topology: F= " << numFaces << " V=" << numVertices << endl;
	cout << " Step1";
	const auto startTime = chrono::high_resolution_clock::now();

	const IntRange vertexRange(0, numVertices);
	const IntRange faceRange(0, numFaces);
	typedef pair<int, int> IntPair;

	/////////////////////////////////////////////////
	//Step1
	vector<int> vertexNumFaces(numVertices, 0);
	{//Step1
		for (const auto fi : faceRange)
		{
			const auto f = faces[fi].m_vis;
			for (int fvi = 0; fvi < 3; ++fvi)
				vertexNumFaces[f[fvi]]++;
		}
	}
	//	int maxNumEdges = numFaces * 3;
	const auto step1EndTime = chrono::high_resolution_clock::now();
	{
		const auto step1Time = chrono::duration<double>(step1EndTime - startTime).count();
		cout << " - " << round(step1Time * 1000) << "ms" << endl;
	}
	/////////////////////////////////////////////////
	//Step2
	cout << " Step2";
	int _maxTotalEdges;
	vector<IntPair> vertexEdgesRange; vertexEdgesRange.reserve(numVertices);
	{//Step2
		int lastIndex = 0;
		{
			for (int vi = 0; vi < numVertices; ++vi)
			{
				vertexEdgesRange.emplace_back(lastIndex, lastIndex);
				lastIndex += vertexNumFaces[vi] * 2;
			}
		}
		_maxTotalEdges = lastIndex;
	}
	const int maxTotalEdges = _maxTotalEdges;

	const auto step2EndTime = chrono::high_resolution_clock::now();
	{
		const auto step2Time = chrono::duration<double>(step2EndTime - step1EndTime).count();
		cout << " - " << round(step2Time * 1000) << "ms" << endl;
	}
	/////////////////////////////////////////////////
	//Step3
	cout << " Step3";
	class EdgeTableEntry final
	{
	public:
		EdgeTableEntry() : m_vi{ -1 }, m_fiC{ -1,-1 }
		{}
		int m_vi;
		array<int, 2> m_fiC;
	};
	vector<EdgeTableEntry> edgeTable(maxTotalEdges);
	{//Step3
		for (int fi = 0; fi < numFaces; ++fi)
		{
			const auto f = faces[fi].m_vis;
			for (int fei = 0; fei < 3; ++fei)
			{
				const auto vi0 = f[fei];
				const auto vi1 = f[p1m3(fei)];
				const auto viS0 = min(vi0, vi1);
				const auto viS1 = max(vi0, vi1);
				const auto fii = (vi0 < vi1) ? 0 : 1;
				auto& etis = vertexEdgesRange[viS0];
				const auto eti = etis.second;
				assert(eti - etis.first < vertexNumFaces[viS0] * 2);
				assert(viS0 >= numVertices - 1 || eti < vertexEdgesRange[viS0 + 1].first);
				++etis.second;
				auto& ete = edgeTable[eti];
				edgeTable[eti].m_vi = viS1;
				edgeTable[eti].m_fiC[fii] = (fi << 2) + fei;
			}
		}
	}
	const auto step3EndTime = chrono::high_resolution_clock::now();
	{
		const auto step3Time = chrono::duration<double>(step3EndTime - step2EndTime).count();
		cout << " - " << round(step3Time * 1000) << "ms" << endl;
	}
	/////////////////////////////////////////////////
	//Step4
	cout << " Step4";
	int numEdgesX = numFaces * 3;
	{ //Step4
		for (int vi = 0; vi < numVertices; ++vi)
		{
			auto& etis = vertexEdgesRange[vi];
			const int start = etis.first;
			int end = etis.second;
			for (int i = start; i < end - 1; ++i)
			{
				const auto eteI = edgeTable[i];
				for (int j = i + 1; j < end; ++j)
				{
					const auto eteJ = edgeTable[j];
					if (eteI.m_vi == eteJ.m_vi)
					{
						for (int k = 0; k < 2; ++k)
						{
							const auto fiE = eteJ.m_fiC[k];
							if (fiE != -1)
								edgeTable[i].m_fiC[k] = fiE;
						}
						--end;
						if (j < end)
							edgeTable[j] = edgeTable[end];
						--numEdgesX;
						break; //We don't check for repeated / triple+ edges
					}
				}
			}
			etis.second = end;
		}
	}
	
	const auto numEdges = numEdgesX;

	const auto step4EndTime = chrono::high_resolution_clock::now();
	{
		const auto step4Time = chrono::duration<double>(step4EndTime - step3EndTime).count();
		cout << " - " << round(step4Time * 1000) << "ms" << endl;
	}
	
	/////////////////////////////////////////////////
	//Step5
	cout << " Step5";
	{//Step5
		faceEdges.resize(numFaces);
		edges.reserve(numEdges);
		allVertexEdgeLists.resize(numEdges * 2);
		vertexEdgeIndicesRanges.resize(numVertices);

		for (int vi = 0; vi < numVertices; ++vi)
		{
			const auto ver = vertexEdgesRange[vi];
			const auto numVertexEdges = ver.second - ver.first;
			vertexEdgeIndicesRanges[vi].m_num += numVertexEdges;
			for (int ii = ver.first; ii < ver.second; ++ii)
				vertexEdgeIndicesRanges[edgeTable[ii].m_vi].m_num += 1;
		}
		{
			int lastIndex = 0;
			for (int vi = 0; vi < numVertices; ++vi)
			{
				auto& veir = vertexEdgeIndicesRanges[vi];
				assert(lastIndex < allVertexEdgeLists.size());
				veir.m_first = lastIndex;
				lastIndex += veir.m_num;
				assert(lastIndex <= allVertexEdgeLists.size());
				veir.m_num = 0;
			}
		}

		int curEdge = 0;
		for (int vi = 0; vi < numVertices; ++vi)
		{
			auto& veir = vertexEdgeIndicesRanges[vi];
			const auto etis = vertexEdgesRange[vi];
			for (int vei = etis.first; vei < etis.second; ++vei)
			{
				const auto ete = edgeTable[vei];
				const auto ei = curEdge; curEdge++;
				const auto ovi = ete.m_vi;
				const array<int, 2> fis = { ete.m_fiC[0] == -1 ? -1 : ete.m_fiC[0] >> 2, ete.m_fiC[1] == -1 ? -1 : ete.m_fiC[1] >> 2 };
				edges.emplace_back(vi, ovi, fis[0], fis[1]);
				{
					const auto o = veir.m_num++;
					const auto wo = veir.m_first + o;
					assert(wo >= veir.m_first);
					if (vi < numVertices - 1)
						assert(wo < vertexEdgeIndicesRanges[vi + 1].m_first);
					else
						assert(wo < allVertexEdgeLists.size());
					allVertexEdgeLists[wo] = ei;
				}
				{
					auto& oveir = vertexEdgeIndicesRanges[ovi];
					const auto o = oveir.m_num++;
					const auto wo = oveir.m_first + o;
					assert(wo >= veir.m_first);
					if (ovi < numVertices - 1)
						assert(wo < vertexEdgeIndicesRanges[ovi + 1].m_first);
					else
						assert(wo < allVertexEdgeLists.size());
					allVertexEdgeLists[oveir.m_first + o] = ei;
				}
				for (int k = 0; k < 2; ++k)
				{
					const auto fi = fis[k];
					if (fi != -1)
					{
						const auto fei = ete.m_fiC[k] & 3;
						faceEdges[fi].m_feis[fei] = ei;
					}
				}
			}
		}
	}
	const auto step5EndTime = chrono::high_resolution_clock::now();
	{
		const auto step5Time = chrono::duration<double>(step5EndTime - step4EndTime).count();
		cout << " - " << round(step5Time * 1000) << "ms" << endl;
		const auto totalTime = chrono::duration<double>(step5EndTime - startTime).count();
		cout << endl << "Total time - " << round(totalTime * 1000) << "ms" << endl;
	}

	cout << endl << " edges: " << numEdges << " / " << numFaces * 3 << " Euler X (V+F-E)=" << numVertices - numEdges + numFaces << endl;
	assert(edges.size() == numEdges);

}

void Mesh::calculateTopology()
{
	calculateTopology1(*this);
}


