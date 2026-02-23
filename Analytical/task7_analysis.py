import numpy as np
import matplotlib.pyplot as plt

meanBlockTime = 2.0  # constant overall block generation rate
nodeHashPower = 20.0 # fixed percentage of hash power for task 7.1

lam = (nodeHashPower * (1.0 / meanBlockTime)) / 100.0 # calculate lambda exactly as we did in node.cc

num_cycles = 100 # total mining cycles to simulate
tk_values = np.random.exponential(scale=(1.0 / lam), size=num_cycles) # generate waiting times using exponential distribution (scale is 1/lambda)

plt.figure(figsize=(12, 5)) # create a wide figure to hold both plots side by side

plt.subplot(1, 2, 1) # first plot: histogram of tk
plt.hist(tk_values, bins=15, density=True, alpha=0.7, color='skyblue', edgecolor='black')
plt.title(f"Waiting Time ($T_k$) Distribution\n(100 Cycles, Hash Power={nodeHashPower}%)")
plt.xlabel("Waiting Time $T_k$ (seconds)")
plt.ylabel("Probability Density")
plt.grid(axis='y', linestyle='--', alpha=0.7)

hash_powers = np.arange(1, 101, 1) # array of test hash powers from 1% to 100%
lambdas = (hash_powers * (1.0 / meanBlockTime)) / 100.0 # calculate lambda for each hash power

plt.subplot(1, 2, 2) # second plot: line graph for lambda vs power
plt.plot(hash_powers, lambdas, color='red', linewidth=2)
plt.title("Lambda ($\lambda$) vs. Hash Power Percentage")
plt.xlabel("Assigned Hash Power (%)")
plt.ylabel("Lambda ($\lambda$) Parameter")
plt.grid(linestyle='--', alpha=0.7)

plt.tight_layout()
plt.show() # display the graphs on screen