//
// Copyright 2021 Pixar
//
// Licensed under the Apache License, Version 2.0 (the "Apache License")
// with the following modification; you may not use this file except in
// compliance with the Apache License and the following modification to it:
// Section 6. Trademarks. is deleted and replaced with:
//
// 6. Trademarks. This License does not grant permission to use the trade
//    names, trademarks, service marks, or product names of the Licensor
//    and its affiliates, except as required to comply with Section 4(c) of
//    the License and to reproduce the content of the NOTICE file.
//
// You may obtain a copy of the Apache License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the Apache License with the above modification is
// distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied. See the Apache License for the specific
// language governing permissions and limitations under the Apache License.
//
#include "hdPrman/hdMaterialNetwork2Interface.h"

PXR_NAMESPACE_OPEN_SCOPE

HdMaterialNode2 *
HdPrmanHdMaterialNetwork2Interface::_GetNode(const TfToken &nodeName) const
{
    if (!_materialNetwork) {
        return nullptr;
    }

    if (nodeName == _lastAccessedNodeName) {
        return _lastAccessedNode;
    }

    _lastAccessedNodeName = nodeName;

    std::map<SdfPath, HdMaterialNode2>::iterator it =
        _materialNetwork->nodes.find(SdfPath(nodeName.data()));


    if (it == _materialNetwork->nodes.end()) {
        _lastAccessedNode  = nullptr;
    } else {
        _lastAccessedNode = &(it->second);
    }

    return _lastAccessedNode;
}

HdMaterialNode2 *
HdPrmanHdMaterialNetwork2Interface::_GetOrCreateNode(const TfToken &nodeName)
    const
{
    HdMaterialNode2 *result = _GetNode(nodeName);
    if (result) {
        return result;
    }

    if (!_materialNetwork) {
        return result;
    }

    _lastAccessedNode = &_materialNetwork->nodes[SdfPath(nodeName.data())];
    return  _lastAccessedNode;
}

TfTokenVector
HdPrmanHdMaterialNetwork2Interface::GetNodeNames() const
{
    TfTokenVector result;
    if (_materialNetwork) {
        result.reserve(_materialNetwork->nodes.size());
        for (const auto nameNodePair : _materialNetwork->nodes) {
            result.push_back(TfToken(nameNodePair.first.GetString()));
        }
    }

    return result;
}

TfToken
HdPrmanHdMaterialNetwork2Interface::GetNodeType(const TfToken &nodeName) const
{
    if (HdMaterialNode2 *node = _GetNode(nodeName)) {
        return node->nodeTypeId;
    }

    return TfToken();
}

TfTokenVector
HdPrmanHdMaterialNetwork2Interface::GetAuthoredNodeParameterNames(
    const TfToken &nodeName) const
{
    TfTokenVector result;
    if (HdMaterialNode2 *node = _GetNode(nodeName)) {
        result.reserve(node->parameters.size());
        for (const auto nameValuePair : node->parameters) {
            result.push_back(nameValuePair.first);
        }
    }
    return result;
}

VtValue
HdPrmanHdMaterialNetwork2Interface::GetNodeParameterValue(
    const TfToken &nodeName,
    const TfToken &paramName) const
{
    if (HdMaterialNode2 *node = _GetNode(nodeName)) {
        const auto it = node->parameters.find(paramName);
        if (it != node->parameters.end()) {
            return it->second;
        }
    }

    return VtValue();
}

TfTokenVector
HdPrmanHdMaterialNetwork2Interface::GetNodeInputConnectionNames(
    const TfToken &nodeName) const
{
    TfTokenVector result;
    if (HdMaterialNode2 *node = _GetNode(nodeName)) {
        result.reserve(node->inputConnections.size());
        for (const auto nameConnectionsPair : node->inputConnections) {
            result.push_back(nameConnectionsPair.first);
        }
    }
    return result;
}

HdPrmanMaterialNetworkInterface::InputConnectionVector
HdPrmanHdMaterialNetwork2Interface::GetNodeInputConnection(
    const TfToken &nodeName,
    const TfToken &inputName) const
{
    InputConnectionVector result;
    if (HdMaterialNode2 *node = _GetNode(nodeName)) {
        const auto it = node->inputConnections.find(inputName);
        if (it != node->inputConnections.end()) {
            result.reserve(it->second.size());
            for (const HdMaterialConnection2 &c : it->second) {
                result.push_back({TfToken(c.upstreamNode.GetString()),
                    c.upstreamOutputName});
            }
        }
    }
    return result;
}

void
HdPrmanHdMaterialNetwork2Interface::DeleteNode(const TfToken &nodeName)
{
    if (_materialNetwork) {
        _materialNetwork->nodes.erase(SdfPath(nodeName.data()));
    }
}

void
HdPrmanHdMaterialNetwork2Interface::SetNodeType(
    const TfToken &nodeName,
    const TfToken &nodeType)
{
    if (HdMaterialNode2 *node = _GetOrCreateNode(nodeName)) {
        node->nodeTypeId = nodeType;
    }
}

void
HdPrmanHdMaterialNetwork2Interface::SetNodeParameterValue(
    const TfToken &nodeName,
    const TfToken &paramName,
    const VtValue &value)
{
    if (HdMaterialNode2 *node = _GetOrCreateNode(nodeName)) {
        node->parameters[paramName] = value;
    }
}

void
HdPrmanHdMaterialNetwork2Interface::DeleteNodeParameter(
    const TfToken &nodeName,
    const TfToken &paramName)
{
    if (HdMaterialNode2 *node = _GetNode(nodeName)) {
        node->parameters.erase(paramName);
    }
}

void
HdPrmanHdMaterialNetwork2Interface::SetNodeInputConnection(
    const TfToken &nodeName,
    const TfToken &inputName,
    const InputConnectionVector &connections)
{
    if (HdMaterialNode2 *node = _GetOrCreateNode(nodeName)) {
        std::vector<HdMaterialConnection2> connections2;
        connections2.reserve(connections.size());
        for (const InputConnection &c : connections) {
            connections2.push_back({SdfPath(c.upstreamNodeName.GetString()),
                c.upstreamOutputName});
        }
        node->inputConnections[inputName] = connections2;
    }
}

void
HdPrmanHdMaterialNetwork2Interface::DeleteNodeInputConnection(
    const TfToken &nodeName,
    const TfToken &inputName)
{
    if (HdMaterialNode2 *node = _GetNode(nodeName)) {
        node->inputConnections.erase(inputName);
    }
}

PXR_NAMESPACE_CLOSE_SCOPE