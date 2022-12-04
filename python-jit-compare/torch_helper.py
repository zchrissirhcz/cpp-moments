# coding: utf-8

import torch
import random
import numpy as np
import torch.nn as nn

# 用来固定随机数种子， 对于单元测试和 debugging 非常有帮助
def setup_seed(seed):
     torch.manual_seed(seed)
     torch.cuda.manual_seed_all(seed)
     np.random.seed(seed)
     random.seed(seed)
     torch.backends.cudnn.deterministic = True
