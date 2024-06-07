# -*- coding: utf-8 -*-


#%% Load Model weights and biases

import numpy as np
import pandas as pd


shape = [[64,3,11,11], [192,64,5,5], [384,192,3,3], [256,384,3,3], [256,256,3,3]]

for n in range(1,6):
  f = open('./data/conv'+ str(n) +'_weight.txt', 'r')
  globals()['conv_weight_'+str(n)] = []
  for line in f:
    s = line.split(' ')
    for i in s:
      globals()['conv_weight_'+str(n)].append(float(i.strip('\n')))
  f.close()
  globals()['conv_weight_'+str(n)] = np.array(globals()['conv_weight_'+str(n)]).reshape(shape[n-1])
  print(globals()['conv_weight_'+str(n)].shape)

  f = open('./data/conv'+ str(n) +'_bias.txt', 'r')
  globals()['conv_bias_'+str(n)] = []
  for line in f:
    s = line.split(' ')
    globals()['conv_bias_'+str(n)].append(float(line))
  f.close()
  globals()['conv_bias_'+str(n)] = np.array(globals()['conv_bias_'+str(n)])
  print(globals()['conv_bias_'+str(n)].shape)

  print('')

shape = [[4096,9216], [4096,4096], [1000,4096]]

for n in range(6,9):
  f = open('./data/fc'+ str(n) +'_weight.txt', 'r')
  globals()['fc_weight_'+str(n)] = []
  for line in f:
    globals()['fc_weight_'+str(n)].append(float(line))
  f.close()
  globals()['fc_weight_'+str(n)] = np.array(globals()['fc_weight_'+str(n)]).reshape(shape[n-6])
  print(globals()['fc_weight_'+str(n)].shape)

  f = open('./data/fc'+ str(n) +'_bias.txt', 'r')
  globals()['fc_bias_'+str(n)] = []
  for line in f:
    globals()['fc_bias_'+str(n)].append(float(line))
  f.close()
  globals()['fc_bias_'+str(n)] = np.array(globals()['fc_bias_'+str(n)])
  print(globals()['fc_bias_'+str(n)].shape)

  print('')


#%% Create the AlexNet model

import torch
import torch.nn as nn
import torch.nn.functional as F

class Net(nn.Module):
    def __init__(self):
        super(Net, self).__init__()
        self.conv1 = nn.Conv2d(3, 64, 11, 4, 2)
        self.conv2 = nn.Conv2d(64, 192, 5, 1, 2)
        self.conv3 = nn.Conv2d(192, 384, 3, 1, 1)
        self.conv4 = nn.Conv2d(384, 256, 3, 1, 1)
        self.conv5 = nn.Conv2d(256, 256, 3, 1, 1)
        self.fc6 = nn.Linear(256*6*6, 4096)
        self.fc7 = nn.Linear(4096, 4096)
        self.fc8 = nn.Linear(4096, 1000)
        self.inter_value = {}

    def forward(self, x):
        x = F.max_pool2d(F.relu(self.conv1(x)), (3, 3), 2)
        self.inter_value['layer1'] = x
        x = F.max_pool2d(F.relu(self.conv2(x)), (3, 3), 2)
        self.inter_value['layer2'] = x
        x = F.relu(self.conv3(x))
        self.inter_value['layer3'] = x
        x = F.relu(self.conv4(x))
        self.inter_value['layer4'] = x
        x = F.max_pool2d(F.relu(self.conv5(x)), (3, 3), 2)
        self.inter_value['layer5'] = x

        x = torch.flatten(x)
        self.inter_value['layer_flat'] = x

        x = F.relu(self.fc6(x))
        self.inter_value['layer6'] = x
        x = F.relu(self.fc7(x))
        self.inter_value['layer7'] = x
        x = self.fc8(x)
        self.inter_value['layer8'] = x
        return x


model = Net()
print(model)

model.conv1.weight.data = torch.from_numpy(globals()['conv_weight_1'])
model.conv1.bias.data = torch.from_numpy(globals()['conv_bias_1'])

model.conv2.weight.data = torch.from_numpy(globals()['conv_weight_2'])
model.conv2.bias.data = torch.from_numpy(globals()['conv_bias_2'])

model.conv3.weight.data = torch.from_numpy(globals()['conv_weight_3'])
model.conv3.bias.data = torch.from_numpy(globals()['conv_bias_3'])

model.conv4.weight.data = torch.from_numpy(globals()['conv_weight_4'])
model.conv4.bias.data = torch.from_numpy(globals()['conv_bias_4'])

model.conv5.weight.data = torch.from_numpy(globals()['conv_weight_5'])
model.conv5.bias.data = torch.from_numpy(globals()['conv_bias_5'])


model.fc6.weight.data=torch.from_numpy(globals()['fc_weight_6'])
model.fc6.bias.data=torch.from_numpy(globals()['fc_bias_6'])

model.fc7.weight.data=torch.from_numpy(globals()['fc_weight_7'])
model.fc7.bias.data=torch.from_numpy(globals()['fc_bias_7'])

model.fc8.weight.data=torch.from_numpy(globals()['fc_weight_8'])
model.fc8.bias.data=torch.from_numpy(globals()['fc_bias_8'])


#%% Load calsses name
f = open('./data/imagenet_classes.txt', 'r')
classes = []
for line in f:
  classes.append(line.strip('\n'))


#%% Load Image and inference
f = open('./data/cat.txt', 'r')
image = []
for line in f:
  s = line.split(' ')
  for i in s:
    g = i.strip('\n')
    image.append(float(g))
image = np.array(image).reshape(3,224,224)
print(image.shape)

#%%
rslt = model(torch.from_numpy(image)).detach().numpy()
# change to df and add index and class name and softmax
rslt = pd.DataFrame(rslt)
rslt = rslt.reset_index()
rslt.columns = ['idx', 'val']
rslt['possibility'] = rslt['val'].apply(lambda x: np.exp(x) / np.sum(np.exp(rslt['val'])))
rslt['class name'] = rslt['idx'].apply(lambda x: classes[x])
# sort using possibility
rslt = rslt.sort_values(by='possibility', ascending=False)
print(rslt.head(5))
# inter value (debug)
for key in model.inter_value.keys():
  model.inter_value[key] = model.inter_value[key].detach().numpy()
golden = model.inter_value

#%% Create the AlexNet model without using any package
# don't use any package like pytorch, tensorflow, keras, etc.
# also don't use any np advanced function like np.convolve, np.dot, etc.
# just use basic np functions like np.zeros, np.maximum, etc.
# do not use vectorization, use for loop to calculate the result


# conv1 nn.Conv2d(3, 64, 11, 4, 2)
#padding=(2,1), stride=4
print('conv1')
conv1_weight = globals()['conv_weight_1']
conv1_bias = globals()['conv_bias_1']
conv1_input = np.zeros((3, 227, 227))
conv1_input[:,2:-1,2:-1] = image
conv1_output = np.zeros((64, 55, 55))
for out_channel in range(64):
  for out_row in range(55):
    for out_col in range(55):
      for in_channel in range(3):
        for ker_row in range(11):
          for ker_col in range(11):
            conv1_output[out_channel, out_row, out_col] += \
              conv1_input[in_channel, out_row*4+ker_row, out_col*4+ker_col] * \
              conv1_weight[out_channel, in_channel, ker_row, ker_col]
      conv1_output[out_channel, out_row, out_col] += conv1_bias[out_channel]
conv1_output = np.maximum(conv1_output, 0)
#pool1 F.max_pool2d(F.relu(self.conv1(x)), (3, 3), 2)
#kernel_size=(3,3), stride=2
print('pool1')
pool1_input = conv1_output
pool1_output = np.zeros((64, 27, 27))
for out_channel in range(64):
  for out_row in range(27):
    for out_col in range(27):
      pool1_output[out_channel, out_row, out_col] = \
        np.max(pool1_input[out_channel, out_row*2:out_row*2+3, out_col*2:out_col*2+3])
#%%
# conv2 nn.Conv2d(64, 192, 5, 1, 2)
#padding=2, stride=1
print('conv2')
conv2_weight = globals()['conv_weight_2']
conv2_bias = globals()['conv_bias_2']
conv2_input = np.zeros((64, 27+4, 27+4))
conv2_input[:,2:-2,2:-2] = pool1_output
conv2_output = np.zeros((192, 27, 27))
for out_channel in range(192):
  for out_row in range(27):
    for out_col in range(27):
      for in_channel in range(64):
        for ker_row in range(5):
          for ker_col in range(5):
            conv2_output[out_channel, out_row, out_col] += \
              conv2_input[in_channel, out_row+ker_row, out_col+ker_col] * \
              conv2_weight[out_channel, in_channel, ker_row, ker_col]
      conv2_output[out_channel, out_row, out_col] += conv2_bias[out_channel]
conv2_output = np.maximum(conv2_output, 0)
#pool2 F.max_pool2d(F.relu(self.conv2(x)), (3, 3), 2)
#kernel_size=(3,3), stride=2
print('pool2')
pool2_input = conv2_output
pool2_output = np.zeros((192, 13, 13))
for out_channel in range(192):
  for out_row in range(13):
    for out_col in range(13):
      pool2_output[out_channel, out_row, out_col] = \
        np.max(pool2_input[out_channel, out_row*2:out_row*2+3, out_col*2:out_col*2+3])
#%%
# conv3 nn.Conv2d(192, 384, 3, 1, 1)
#padding=1, stride=1
print('conv3')
conv3_weight = globals()['conv_weight_3']
conv3_bias = globals()['conv_bias_3']
conv3_input = np.zeros((192, 13+2, 13+2))
conv3_input[:,1:-1,1:-1] = pool2_output
conv3_output = np.zeros((384, 13, 13))
for out_channel in range(384):
  for out_row in range(13):
    for out_col in range(13):
      for in_channel in range(192):
        for ker_row in range(3):
          for ker_col in range(3):
            conv3_output[out_channel, out_row, out_col] += \
              conv3_input[in_channel, out_row+ker_row, out_col+ker_col] * \
              conv3_weight[out_channel, in_channel, ker_row, ker_col]
      conv3_output[out_channel, out_row, out_col] += conv3_bias[out_channel]
conv3_output = np.maximum(conv3_output, 0)
#%%
# conv4 nn.Conv2d(384, 256, 3, 1, 1)
#padding=1, stride=1
print('conv4')
conv4_weight = globals()['conv_weight_4']
conv4_bias = globals()['conv_bias_4']
conv4_input = np.zeros((384, 13+2, 13+2))
conv4_input[:,1:-1,1:-1] = conv3_output
conv4_output = np.zeros((256, 13, 13))
for out_channel in range(256):
  for out_row in range(13):
    for out_col in range(13):
      for in_channel in range(384):
        for ker_row in range(3):
          for ker_col in range(3):
            conv4_output[out_channel, out_row, out_col] += \
              conv4_input[in_channel, out_row+ker_row, out_col+ker_col] * \
              conv4_weight[out_channel, in_channel, ker_row, ker_col]
      conv4_output[out_channel, out_row, out_col] += conv4_bias[out_channel]
conv4_output = np.maximum(conv4_output, 0)
#%%
# conv5 nn.Conv2d(256, 256, 3, 1, 1)
#padding=1, stride=1
print('conv5')
conv5_weight = globals()['conv_weight_5']
conv5_bias = globals()['conv_bias_5']
conv5_input = np.zeros((256, 13+2, 13+2))
conv5_input[:,1:-1,1:-1] = conv4_output
conv5_output = np.zeros((256, 13, 13))
for out_channel in range(256):
  for out_row in range(13):
    for out_col in range(13):
      for in_channel in range(256):
        for ker_row in range(3):
          for ker_col in range(3):
            conv5_output[out_channel, out_row, out_col] += \
              conv5_input[in_channel, out_row+ker_row, out_col+ker_col] * \
              conv5_weight[out_channel, in_channel, ker_row, ker_col]
      conv5_output[out_channel, out_row, out_col] += conv5_bias[out_channel]
conv5_output = np.maximum(conv5_output, 0)
# pool5 F.max_pool2d(F.relu(self.conv5(x)), (3, 3), 2)
#kernel_size=(3,3), stride=2
print('pool5')
pool5_input = conv5_output
pool5_output = np.zeros((256, 6, 6))
for out_channel in range(256):
  for out_row in range(6):
    for out_col in range(6):
      pool5_output[out_channel, out_row, out_col] = \
        np.max(pool5_input[out_channel, out_row*2:out_row*2+3, out_col*2:out_col*2+3])
#%%
# aptive_avg_pool2d
#kernal=(3,3), stride=2
# print('adaptive_avg')
# avg_input = np.zeros((256, 6+2, 6+2))
# avg_input[:,1:-1,1:-1] = pool5_output
# avg_output = np.zeros((256, 6, 6))
# for out_channel in range(256):
#   for out_row in range(6):
#     for out_col in range(6):
#       avg_output[out_channel, out_row, out_col] = \
#         np.mean(avg_input[out_channel, out_row:out_row+3, out_col:out_col+3].flatten())
#%%
# fc6 nn.Linear(256*6*6, 4096)
print('fc6')
fc6_weight = globals()['fc_weight_6']
fc6_bias = globals()['fc_bias_6']
fc6_input = np.zeros(256*6*6)
fc6_input = pool5_output.flatten()
fc6_output = np.zeros(4096)
for out_channel in range(4096):
  for in_channel in range(256*6*6):
    fc6_output[out_channel] += fc6_input[in_channel] * fc6_weight[out_channel, in_channel]
  fc6_output[out_channel] += fc6_bias[out_channel]
fc6_output = np.maximum(fc6_output, 0)
# fc7 nn.Linear(4096, 4096)
print('fc7')
fc7_weight = globals()['fc_weight_7']
fc7_bias = globals()['fc_bias_7']
fc7_input = np.zeros(4096)
fc7_input = fc6_output
fc7_output = np.zeros(4096)
for out_channel in range(4096):
  for in_channel in range(4096):
    fc7_output[out_channel] += fc7_input[in_channel] * fc7_weight[out_channel, in_channel]
  fc7_output[out_channel] += fc7_bias[out_channel]
fc7_output = np.maximum(fc7_output, 0)
# fc8 nn.Linear(4096, 1000)
print('fc8')
fc8_weight = globals()['fc_weight_8']
fc8_bias = globals()['fc_bias_8']
fc8_input = np.zeros(4096)
fc8_input = fc7_output
fc8_output = np.zeros(1000)
for out_channel in range(1000):
  for in_channel in range(4096):
    fc8_output[out_channel] += fc8_input[in_channel] * fc8_weight[out_channel, in_channel]
  fc8_output[out_channel] += fc8_bias[out_channel]




#%%
self_rslt = fc8_output
# change to df and add index and class name and softmax
self_rslt = pd.DataFrame(self_rslt)
self_rslt = self_rslt.reset_index()
self_rslt.columns = ['idx', 'val']
self_rslt['possibility'] = self_rslt['val'].apply(lambda x: np.exp(x) / np.sum(np.exp(self_rslt['val'])))
self_rslt['class name'] = self_rslt['idx'].apply(lambda x: classes[x])
# sort using possibility
self_rslt = self_rslt.sort_values(by='possibility', ascending=False)
print(self_rslt.head(5))





#%% Load Image and inference using self_alexnet
# self_rslt = self_alexnet(image)

