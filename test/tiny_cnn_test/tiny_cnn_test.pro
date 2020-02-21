TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

include(../../image_recognizing/opencv/opencv.pri)

SOURCES += \
    ../tiny_cnn_train/examples/mnist/test.cpp

HEADERS += \
    tiny_cnn/activations/activation_function.h \
    tiny_cnn/io/caffe/layer_factory.h \
    tiny_cnn/io/caffe/layer_factory_impl.h \
    tiny_cnn/io/cifar10_parser.h \
    tiny_cnn/io/display.h \
    tiny_cnn/io/layer_factory.h \
    tiny_cnn/io/mnist_parser.h \
    tiny_cnn/layers/arithmetic_layer.h \
    tiny_cnn/layers/average_pooling_layer.h \
    tiny_cnn/layers/average_unpooling_layer.h \
    tiny_cnn/layers/concat_layer.h \
    tiny_cnn/layers/convolutional_layer.h \
    tiny_cnn/layers/deconvolutional_layer.h \
    tiny_cnn/layers/dropout_layer.h \
    tiny_cnn/layers/feedforward_layer.h \
    tiny_cnn/layers/fully_connected_layer.h \
    tiny_cnn/layers/input_layer.h \
    tiny_cnn/layers/layer.h \
    tiny_cnn/layers/layers.h \
    tiny_cnn/layers/linear_layer.h \
    tiny_cnn/layers/lrn_layer.h \
    tiny_cnn/layers/max_pooling_layer.h \
    tiny_cnn/layers/max_unpooling_layer.h \
    tiny_cnn/layers/partial_connected_layer.h \
    tiny_cnn/lossfunctions/loss_function.h \
    tiny_cnn/optimizers/optimizer.h \
    tiny_cnn/util/aligned_allocator.h \
    tiny_cnn/util/conv_kernel.h \
    tiny_cnn/util/deform.h \
    tiny_cnn/util/graph_visualizer.h \
    tiny_cnn/util/image.h \
    tiny_cnn/util/nn_error.h \
    tiny_cnn/util/product.h \
    tiny_cnn/util/target_cost.h \
    tiny_cnn/util/util.h \
    tiny_cnn/util/weight_init.h \
    tiny_cnn/config.h \
    tiny_cnn/network.h \
    tiny_cnn/node.h \
    tiny_cnn/nodes.h \
    tiny_cnn/tiny_cnn.h
