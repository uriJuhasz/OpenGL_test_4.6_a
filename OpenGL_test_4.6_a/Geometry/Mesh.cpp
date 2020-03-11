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
			if (fi == 45204)
			{
				cout << "";
			}
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
	vector<array<int, 2>> duplicatedEdges;
	{ //Step4
		for (int vi = 0; vi < numVertices; ++vi)
		{
			auto& etis = vertexEdgesRange[vi];
			const int start = etis.first;
			int end = etis.second;
			for (int i = start; i < end - 1; ++i)
			{
				const auto eteI = edgeTable[i];
				if (vi == 5435 && eteI.m_vi == 52695)
				{
					cout << "";
				}
				for (int j = i + 1; j < end; ++j)
				{
					const auto eteJ = edgeTable[j];
					if (eteI.m_vi == eteJ.m_vi)
					{
						for (int k = 0; k < 2; ++k)
						{
							const auto fiE = eteJ.m_fiC[k];
							if (fiE != -1)
							{
								if (edgeTable[i].m_fiC[k] == -1)
								{
									edgeTable[i].m_fiC[k] = fiE;
								}
								else
								{
									const auto fiI = edgeTable[i].m_fiC[k] >> 2;
									const auto feiI = edgeTable[i].m_fiC[k] & 3;
									const auto fiJ = edgeTable[j].m_fiC[k] >> 2;
									const auto feiJ = edgeTable[j].m_fiC[k] & 3;
									const auto& fI = faces[fiI].m_vis;
									const auto& fJ = faces[fiJ].m_vis;
									cerr << " Warning - triangle edges "
										<< fiI << ":" << feiI << " [" << fI[0] << " " << fI[1] << " " << fI[2] << "]"
										<< " and "
										<< fiJ << ":" << feiJ << " [" << fJ[0] << " " << fJ[1] << " " << fJ[2] << "]"
										<< " overlap on edge " << (k ? vi : eteJ.m_vi) << " - " << (k ? eteJ.m_vi : vi)
										<< endl;
									duplicatedEdges.emplace_back(array<int, 2>{edgeTable[i].m_fiC[k], edgeTable[j].m_fiC[k]});
								}
							}
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
					const auto fei = ete.m_fiC[k] & 3;
					if (fi != -1)
					{
						faceEdges[fi].m_feis[fei] = ei;
					}
				}
			}
		}
		for (const auto de : duplicatedEdges)
		{
			const auto fi0 = de[0] >> 2;
			const auto fei0 = de[0] & 3;
			const auto fi1 = de[1] >> 2;
			const auto fei1 = de[1] & 3;
			assert(faceEdges[fi1].m_feis[fei1] == -1);
			faceEdges[fi1].m_feis[fei1] = faceEdges[fi0].m_feis[fei0];
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
	validateTopology();
}

#ifdef _DEBUG
#define alwaysAssert(b) assert(b)
#else
void alwaysAssert(const bool b)
{
	assert(b);
	if (!b)
		throw new int();
}
#endif

template<class C, class T> bool rangeContains(const C& c, const int start, const int end, const T v)
{
	for (int i = start; i < end; ++i)
		if (c[i] == v)
			return true;
	return false;
}
template<class C, class T> bool contains(const C& c, const T v)
{
	return rangeContains(c, 0, c.size(), v);
}
void Mesh::validateTopology()
{
	const auto& faces = m_faces;
	const auto& edges = m_edges;
	const auto F = numFaces();
	const auto V = numVertices();
	const int E = toInt(edges.size());

	for (int fi = 0; fi < F; ++fi)
	{
		const auto& fvis = m_faces[fi].m_vis;
		const auto& feis = m_faceEdges[fi].m_feis;
		for (int fei = 0; fei < 3; ++fei)
		{
			const auto ei = feis[fei];
			alwaysAssert(0 <= ei && ei < E);
			const auto& e = m_edges[ei];
			const auto vi0 = fvis[fei];
			const auto vi1 = fvis[p1m3(fei)];
			const auto pi = (vi0 <= vi1) ? 0 : 1;
			const array<int, 2> fviS{ std::min(vi0, vi1), std::max(vi0, vi1) };
			alwaysAssert(e.m_vis == fviS);
//			alwaysAssert(e.m_fis[pi] == fi); Does not hold for duplicate edges
			alwaysAssert(e.m_fis[pi] != -1);
		}
	}

	const auto& vertexEdgeIndicesRanges = m_vertexEdgeIndicesRanges;
	const auto& allVertexEdgeLists = m_allVertexEdgeLists;
	for (int vi = 0; vi < V; ++vi)
	{
		const auto veir = vertexEdgeIndicesRanges[vi];
		const auto first = veir.m_first;
		alwaysAssert(0 <= first && first < allVertexEdgeLists.size());
		const auto num = veir.m_num;
		const auto end = first + num;
		alwaysAssert(0 <= end&& end <= allVertexEdgeLists.size());
		alwaysAssert(0 <= first);
		if (0 < vi)
			alwaysAssert(vertexEdgeIndicesRanges[vi - 1].m_first + vertexEdgeIndicesRanges[vi - 1].m_num == first);

		if (vi<V-1)
			alwaysAssert(end == vertexEdgeIndicesRanges[vi + 1].m_first);
		for (int vei = first; vei < end; ++vei)
		{
			const auto ei = allVertexEdgeLists[vei];
			alwaysAssert(0 <= ei && ei < E);
			const auto& e = edges[ei];
			alwaysAssert(e.m_vis[0] == vi || e.m_vis[1] == vi);
			for (int j = vei + 1; j < end; ++j)
				alwaysAssert(allVertexEdgeLists[j] != ei);//unique edge
		}
	}

	for (int ei = 0; ei < E; ++ei)
	{
		const auto& e = edges[ei];
		assert(e.m_vis[0] < e.m_vis[1]);
		for (int evi = 0; evi < 2; ++evi)
		{
			const auto vi = e.m_vis[evi];
			alwaysAssert(0 <= vi && vi < V);
			const auto veir = vertexEdgeIndicesRanges[vi];
			const auto end = veir.m_first + veir.m_num;
			alwaysAssert(rangeContains(allVertexEdgeLists, veir.m_first, end, ei));
		}
		for (int efi = 0; efi < 2; ++efi)
		{
			const auto fi = e.m_fis[efi];
			if (fi != -1)
			{
				assert(0 <= fi && fi < F);
				const auto& feis = m_faceEdges[fi].m_feis;
				assert(contains(feis, ei));
			}
		}
	}
}