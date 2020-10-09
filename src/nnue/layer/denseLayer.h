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

#ifndef _DENSE_LAYER_H
#define _DENSE_LAYER_H

#include <vector>
#include "nnue_type.h"
#include "layer.h"

class Activation;

class DenseLayer: public Layer {
public:
    enum class activationType{
        linear,
        relu
    };
    
    DenseLayer(const unsigned int inputSize, const unsigned int outputSize, activationType act, std::vector<biasType>* bias, std::vector<weightType>* weight, unsigned int biasScale, unsigned int weightScale, unsigned int outShift);
    ~DenseLayer();

    void propagate(const FeatureList& l, const FeatureList& h);
    void incrementalPropagate(const DifferentialList& l, const DifferentialList& h);

    void propagate(const std::vector<outType>& input);
    int32_t propagate(const std::vector<outType>& input, const unsigned int index, unsigned int o);

    unsigned int _calcWeightIndex(const unsigned int i, const unsigned int o) const;

    bool deserialize(std::ifstream& ss);

    const std::vector<outType>& output() const;
    
private:
    std::vector<biasType>* _bias;
    std::vector<weightType>* _weight;
    
    activationType _act;

    std::vector<outType> _output;

    /*double _max = -1e9;
    double _min = 1e9;*/
};

#endif  
