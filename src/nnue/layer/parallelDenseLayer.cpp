/*
	This file is part of Vajolet.
	Copyright (C) 2013-2018 Marco Belli
	
    Vajolet is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Vajolet is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Vajolet.  If not, see <http://www.gnu.org/licenses/>
*/

#include <cassert>
#include <iostream>

#include "featureList.h"
#include "differentialList.h"
#include "parallelDenseLayer.h"

ParallelDenseLayer::ParallelDenseLayer(const unsigned int inputSize, const unsigned int outputSize, std::vector<std::vector<double>*> biases, std::vector<std::vector<double>*> weights):
    Layer{2 * inputSize, 2 * outputSize}, 
    _layerOutputSize(outputSize),
    _bias0(biases[0]),
    _bias1(biases[1]),
    _weight0(weights[0]),
    _weight1(weights[1])
{}

ParallelDenseLayer::~ParallelDenseLayer() {}

unsigned int ParallelDenseLayer::_calcWeightIndex(const unsigned int i, const unsigned int o) const {
    assert(o + i * _layerOutputSize < _weight0->size());
    return o + i * _layerOutputSize;

}

void ParallelDenseLayer::propagate(const FeatureList& l, const FeatureList& h) {
    for (unsigned int o = 0; o < _layerOutputSize; ++o) {
        _output[o] = (*_bias0)[o];
    }

    for (unsigned int o = 0; o < _layerOutputSize; ++o) {
        _output[_layerOutputSize + o ] = (*_bias1)[o];
    }

    for (unsigned int idx = 0; idx < l.size(); ++idx) {
        unsigned int in = l.get(idx);
        for (unsigned int o = 0; o < _layerOutputSize; ++o) {
            _output[o] += (*_weight0)[_calcWeightIndex(in, o)];
        }
    } 

    for (unsigned int idx = 0; idx < h.size(); ++idx) {
        unsigned int in = h.get(idx);
        for (unsigned int o = 0; o < _layerOutputSize; ++o) {
            _output[_layerOutputSize + o] += (*_weight1)[_calcWeightIndex(in, o)];
        }
    } 
    
}

void ParallelDenseLayer::incrementalPropagate(const DifferentialList& l, const DifferentialList& h) {
    for(unsigned int idx = 0; idx < l.addSize(); ++idx) {
        unsigned int in = l.addList(idx);
        for (unsigned int o = 0; o < _layerOutputSize; ++o) {
            _output[o] += (*_weight0)[_calcWeightIndex(in, o)];
        }
    }

    for(unsigned int idx = 0; idx < l.removeSize(); ++idx) {
        unsigned int in = l.removeList(idx);
        for (unsigned int o = 0; o < _layerOutputSize; ++o) {
            _output[o] -= (*_weight0)[_calcWeightIndex(in, o)];
        }
    }

    for(unsigned int idx = 0; idx < h.addSize(); ++idx) {
        unsigned int in = h.addList(idx);
        for (unsigned int o = 0; o < _layerOutputSize; ++o) {
            _output[_layerOutputSize + o] += (*_weight1)[_calcWeightIndex(in, o)];
        }
    }

    for(unsigned int idx = 0; idx < h.removeSize(); ++idx) {
        unsigned int in = h.removeList(idx);
        for (unsigned int o = 0; o < _layerOutputSize; ++o) {
            _output[_layerOutputSize + o] -= (*_weight1)[_calcWeightIndex(in, o)];
        }
    }
}

void ParallelDenseLayer::propagate(const std::vector<double>& ) {
    std::cout<<"ARGHHHHH"<<std::endl;
}

bool ParallelDenseLayer::deserialize(std::ifstream& ss) {
    //std::cout<<"DESERIALIZE PARALLEL DENSE LAYER"<<std::endl;
    if(ss.get() != '{') {std::cout<<"ParallelDenseLayer missing {"<<std::endl;return false;}
    
    if(!_deserialize(ss, _bias0, _weight0)) {std::cout<<"ParallelDenseLayer internal layer error"<<std::endl;return false;}
    if(!_deserialize(ss, _bias1, _weight1)) {std::cout<<"ParallelDenseLayer internal layer error"<<std::endl;return false;}
    
    if(ss.get() != '}') {std::cout<<"ParallelDenseLayer missing }"<<std::endl;return false;}
    if(ss.get() != '\n') {std::cout<<"DenseLayer missing line feed"<<std::endl;return false;}
    return true;
}

bool ParallelDenseLayer::_deserialize(std::ifstream& ss, std::vector<double>* bias, std::vector<double>* weight) {
    //std::cout<<"DESERIALIZE DENSE LAYER"<<std::endl;
    union un{
        double d;
        char c[8];
    }u;
    if(ss.get() != '{') {std::cout<<"DenseLayer missing {"<<std::endl;return false;}
    for( auto & b: *bias) {
        ss.read(u.c, 8);
        b = u.d;
        if(ss.get() != ',') {std::cout<<"DenseLayer missing ,"<<std::endl;return false;} 
        if(ss.get() != ' ') {std::cout<<"DenseLayer missing space"<<std::endl;return false;}
    }
    if(ss.get() != '\n') {std::cout<<"DenseLayer missing line feed"<<std::endl;return false;}
    for( auto & w: *weight) {
        ss.read(u.c, 8);
        w = u.d;
        if(ss.get() != ',') {std::cout<<"DenseLayer missing ,"<<std::endl;return false;} 
        if(ss.get() != ' ') {std::cout<<"DenseLayer missing space"<<std::endl;return false;}
    }
    if(ss.get() != '}') {std::cout<<"DenseLayer missing }"<<std::endl;return false;} 
    if(ss.get() != '\n') {std::cout<<"DenseLayer missing line feed"<<std::endl;return false;}
    return true;
}
