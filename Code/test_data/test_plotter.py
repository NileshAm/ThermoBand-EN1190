import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import statsmodels.api as sm

# Read the CSV file
data = pd.read_csv('./Code/test_output_data.csv')[1:]

initialTime = data.iloc[0, 0]
# print(initialTime)

def timeDiff(end, start):
    endH, endM, endS = end.split(":")
    startH, startM, startS = start.split(":")

    return round((int(endH) - int(startH)) * 3600 + (int(endM) - int(startM)) * 60 + (int(endS.split(".")[0]) - int(startS.split(".")[0])))

data[data.columns[0]] = data[data.columns[0]].apply(lambda d: timeDiff(d.split("T")[1], initialTime.split("T")[1]))

actual = np.array([36.5,36.5,36.6,36.5,36.5,36.5,36.6,36.1,36.4,36.5,36.5,36.1,36.5])
readData = np.array(data[data.columns[1]])

meanoffset = round(np.average(actual) - np.average(data[data.columns[1]]), 3)

X = sm.add_constant(readData)
model = sm.OLS(actual, X).fit()

offset, slope = model.params

print(f"mean offset = {meanoffset}")
print(f"Intercept (offset) = {offset:.3f} °C")
print(f"Slope = {slope:.5f}")
print(f"R² = {model.rsquared:.4f}")
print(f"RMSE = {(model.resid**2).mean()**0.5:.3f} °C")

# y = [(timeDiff(d.split("T")[1], initialTime.split("T")[1])) for d in data[data.columns[0]]]
plt.plot(data[data.columns[0]], data[data.columns[1]], label="Measure data from the wrist")
plt.plot(data[data.columns[0]], np.array(data[data.columns[1]])+meanoffset, label=f"Measure data from the wrist with {meanoffset}$\degree$C")
plt.plot(data[data.columns[0]], slope * data[data.columns[1]] + offset, label=f"Regression model data Slope={slope:.2f} offset={offset:.2f}")
plt.plot(data[data.columns[0]], actual, label="Measured from a commertial Thermometer")
plt.plot(data[data.columns[0]], [37.2]*len(actual), label="Upper limit of healthy temp", linestyle="dashed")
plt.plot(data[data.columns[0]], [36.1]*len(actual), label="Lower limit of healthy temp", linestyle="dashed")
# plt.plot(data[data.columns[0]], actual-data[data.columns[1]]-offset, label="difference", linestyle="dashed")

# plt.plot(data[data.columns[0]], np.array(data[data.columns[1]])+offset-actual)
# Add labels, title, and legend
plt.xlabel("Time (s)")
plt.ylabel('$Temperature (\degree C)$')
# plt.yticks(np.arange(31.5, 37.75, 0.25))    
plt.xticks(np.arange(0,4300, 250))
plt.title('Test Output Data')
# plt.legend([f"Tempurature Recorded with {offset}$\degree$C offset","Tempurature Recorded(Read at top side of the wrist)", "Commertially available thermometer(Read orally)"])
plt.legend()
plt.grid(True, linestyle="--")

# Show the plot
plt.show()