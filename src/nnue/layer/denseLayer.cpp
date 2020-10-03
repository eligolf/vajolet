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

#include "denseLayer.h"
#include "input.h"

DenseLayer::DenseLayer(const unsigned int inputSize, const unsigned int outputSize, activationType act, std::vector<double>* bias, std::vector<double>* weight):
    Layer{inputSize, outputSize},
    _bias(bias),
    _weight(weight),
    _act(act)
    
{
    _netOutput.resize(outputSize, 0.0);
}

DenseLayer::~DenseLayer() {}

void DenseLayer::_calcNetOut1(const Input& input, bool incremental) {
    assert(input.size() == _inputSize);
    if (!incremental) {
        _netOutput = *_bias;
    }
    unsigned int num = input.getElementNumber();
    //if (incremental)std::cout<<"element number "<< num<<std::endl;
    for (unsigned int idx = 0; idx < num; ++idx) {
        auto& el = input.getElementFromIndex(idx);
        //if (incremental)std::cout<<"element idx "<< idx<<" index "<<el.first<< " value "<<el.second<<std::endl;
        for (unsigned int o = 0; o < _outputSize; ++o) {
            _netOutput[o] += el.second * (*_weight)[_calcWeightIndex(el.first,o)];
        }
    }  
}

void DenseLayer::_calcNetOut2(const std::vector<double>& input, bool incremental) {
    assert(input.size() == _inputSize);
    if (!incremental) {
        _netOutput = *_bias;
    }
    unsigned int index = 0;
    for(double value: input) {
        for (unsigned int o = 0; o < _outputSize; ++o) {
            _netOutput[o] += value * (*_weight)[_calcWeightIndex(index, o)];
        }
        ++index;
    }
}

void DenseLayer::_calcOut() {
    if(_act == activationType::relu) {
        for(unsigned int o=0; o < _outputSize; ++o) {
            _output[o] = std::max(_netOutput[o], 0.0); 
        }
    }
    else {
        //todo write directly to _output
        _output = _netOutput;
    }
}

void DenseLayer::propagate(const Input& input) {
    _calcNetOut1(input);
    _calcOut();
}

void DenseLayer::incrementalPropagate(const Input& input) {
    _calcNetOut1(input, true);
    _calcOut();
}

void DenseLayer::propagate(const std::vector<double>& input) {
    _calcNetOut2(input);
    _calcOut();
}

void DenseLayer::incrementalPropagate(const std::vector<double>& input) {
    _calcNetOut2(input, true);
    _calcOut();
}

unsigned int DenseLayer::_calcWeightIndex(const unsigned int i, const unsigned int o) const {
    assert(o + i * _outputSize < _weight->size());
    return o + i * _outputSize;

}

bool DenseLayer::deserialize(std::ifstream& ss) {
    //std::cout<<"DESERIALIZE DENSE LAYER"<<std::endl;
    union un{
        double d;
        char c[8];
    }u;
    if(ss.get() != '{') {std::cout<<"DenseLayer missing {"<<std::endl;return false;}
    for( auto & b: *_bias) {
        ss.read(u.c, 8);
        b = u.d;
        if(ss.get() != ',') {std::cout<<"DenseLayer missing ,"<<std::endl;return false;} 
        if(ss.get() != ' ') {std::cout<<"DenseLayer missing space"<<std::endl;return false;}
    }
    if(ss.get() != '\n') {std::cout<<"DenseLayer missing line feed"<<std::endl;return false;}
    for( auto & w: *_weight) {
        ss.read(u.c, 8);
        w = u.d;
        if(ss.get() != ',') {std::cout<<"DenseLayer missing ,"<<std::endl;return false;} 
        if(ss.get() != ' ') {std::cout<<"DenseLayer missing space"<<std::endl;return false;}
    }
    if(ss.get() != '}') {std::cout<<"DenseLayer missing }"<<std::endl;return false;} 
    if(ss.get() != '\n') {std::cout<<"DenseLayer missing line feed"<<std::endl;return false;}
    return true;
}
