/*
 * NormalDepthMap.cpp
 *
 *  Created on: Mar 27, 2015
 *      Author: tiagotrocoli
 */

#include "NormalDepthMap.hpp"

#include <osg/Program>
#include <osg/ref_ptr>
#include <osg/Shader>
#include <osg/StateSet>
#include <osg/Uniform>
#include <osgDB/FileUtils>
#include <osg/ShapeDrawable>
#include <iostream>


namespace normal_depth_map {

#define SHADER_PATH_FRAG "normal_depth_map/shaders/normalDepthMap.frag"
#define SHADER_PATH_VERT "normal_depth_map/shaders/normalDepthMap.vert"

// #define SHADER_PATH_FRAG "normal_depth_map/shaders/temp/reverberation.frag"
// #define SHADER_PATH_VERT "normal_depth_map/shaders/temp/reverberation.vert"


TrianglesVisitor::TrianglesVisitor() : meshs(0) {
    setTraversalMode( osg::NodeVisitor::TRAVERSE_ALL_CHILDREN );
};

void TrianglesVisitor::apply( osg::Geode& geode ) {

    for ( unsigned int i = 0; i< geode.getNumDrawables(); ++i ) {
        triangles_data.local_2_world = osg::computeLocalToWorld(getNodePath());
        geode.getDrawable(i)->accept(triangles_data);

        // std::cout << "Node: " << meshs++ <<" | "
        //           << "Triangles:centroid " << triangles_data.triangles->size();
        //
        // for (unsigned int j = 0; j < triangles_data.triangles->size(); j++) {
        //     if ( (j % 500) == 0 )
        //         std::cout << "\n i = " << j << " | CENTROID ["
        //                   << (*triangles_data.triangles)[j]._centroid.x() << ", "
        //                   << (*triangles_data.triangles)[j]._centroid.y() << ", "
        //                   << (*triangles_data.triangles)[j]._centroid.z() << "]";
        // }
        //
        // std::cout << std::endl;
    }
}



NormalDepthMap::NormalDepthMap(float maxRange ) {
    _normalDepthMapNode = createTheNormalDepthMapShaderNode(maxRange);
}

NormalDepthMap::NormalDepthMap(float maxRange, float attenuationCoeff) {
    _normalDepthMapNode = createTheNormalDepthMapShaderNode(
                                                          maxRange,
                                                          attenuationCoeff);
}

NormalDepthMap::NormalDepthMap() {
    _normalDepthMapNode = createTheNormalDepthMapShaderNode();
}

void NormalDepthMap::setMaxRange(float maxRange) {
    _normalDepthMapNode->getOrCreateStateSet()
                       ->getUniform("farPlane")
                       ->set(maxRange);
}

float NormalDepthMap::getMaxRange() {
    float maxRange = 0;
    _normalDepthMapNode->getOrCreateStateSet()
                       ->getUniform("farPlane")
                       ->get(maxRange);
    return maxRange;
}

void NormalDepthMap::setAttenuationCoefficient(float coefficient) {
    _normalDepthMapNode->getOrCreateStateSet()
                       ->getUniform("attenuationCoeff")
                       ->set(coefficient);
}

float NormalDepthMap::getAttenuationCoefficient() {
    float coefficient = 0;
    _normalDepthMapNode->getOrCreateStateSet()
                       ->getUniform("attenuationCoeff")
                       ->get(coefficient);
    return coefficient;
}

void NormalDepthMap::setDrawNormal(bool drawNormal) {
    _normalDepthMapNode->getOrCreateStateSet()
                       ->getUniform("drawNormal")
                       ->set(drawNormal);
}

bool NormalDepthMap::isDrawNormal() {
    bool drawNormal;
    _normalDepthMapNode->getOrCreateStateSet()
                       ->getUniform("drawNormal")
                       ->get(drawNormal);
    return drawNormal;
}

void NormalDepthMap::setDrawDepth(bool drawDepth) {
    _normalDepthMapNode->getOrCreateStateSet()
                       ->getUniform("drawDepth")
                       ->set(drawDepth);
}

bool NormalDepthMap::isDrawDepth() {
    bool drawDepth;
    _normalDepthMapNode->getOrCreateStateSet()
                       ->getUniform("drawDepth")
                       ->get(drawDepth);
    return drawDepth;
}

void NormalDepthMap::addNodeChild(osg::ref_ptr<osg::Node> node) {
    _normalDepthMapNode->addChild(node);
    TrianglesVisitor visitor;
    _normalDepthMapNode->accept(visitor);
}

osg::ref_ptr<osg::Group> NormalDepthMap::createTheNormalDepthMapShaderNode(
                                                float maxRange,
                                                float attenuationCoefficient,
                                                bool drawDepth,
                                                bool drawNormal) {

    osg::ref_ptr<osg::Group> localRoot = new osg::Group();
    osg::ref_ptr<osg::Program> program(new osg::Program());

    osg::ref_ptr<osg::Shader> shaderVertex = osg::Shader::readShaderFile(
                                              osg::Shader::VERTEX,
                                              osgDB::findDataFile(
                                                             SHADER_PATH_VERT));

    osg::ref_ptr<osg::Shader> shaderFragment = osg::Shader::readShaderFile(
                                               osg::Shader::FRAGMENT,
                                               osgDB::findDataFile(
                                                             SHADER_PATH_FRAG));
    program->addShader(shaderFragment);
    program->addShader(shaderVertex);

    osg::ref_ptr<osg::StateSet> ss = localRoot->getOrCreateStateSet();
    ss->setAttribute(program);

    ss->addUniform(new osg::Uniform("farPlane", maxRange));
    ss->addUniform(new osg::Uniform("attenuationCoeff",
                                    attenuationCoefficient));
    ss->addUniform(new osg::Uniform("drawNormal", drawNormal));
    ss->addUniform(new osg::Uniform("drawDepth", drawDepth));

    return localRoot;
}

void NormalDepthMap::convertVecticesToTexture(osg::ref_ptr<osg::Node> node){

}

}
