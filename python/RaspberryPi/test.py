import unittest
import math
from ecsalinity import ecsalinity

ec = ecsalinity(3)


class TestMethods(unittest.TestCase):

    def test_version(self):
        self.assertEqual(ec.getVersion(), 0x1c)

    def test_temp_constant(self):
        ec.setTempConstant(20)
        self.assertEqual(ec.getTempConstant(), 20)

    def test_k(self):
        ec.setK(1.01)
        self.assertEqual(ec.getK(), 1.01)

    def test_set_calibrate(self):
        ec.setCalibrateOffset(1.01)
        self.assertEqual(ec.getCalibrateOffset(), 1.01)

    def test_use_temp_comp_true(self):
        ec.useTemperatureCompensation(True)
        self.assertEqual(ec.usingTemperatureCompensation(), True)

    def test_use_temp_comp_false(self):
        ec.useTemperatureCompensation(False)
        self.assertEqual(ec.usingTemperatureCompensation(), False)

    def test_use_dual_point_true(self):
        ec.useDualPoint(True)
        self.assertEqual(ec.usingDualPoint(), True)

    def test_use_dual_point_false(self):
        ec.useDualPoint(False)
        self.assertEqual(ec.usingDualPoint(), False)

    def test_reference_low(self):
        ec.setDualPointCalibration(1.01, 0, 0, 0)
        self.assertEqual(ec.getCalibrateLow(), 1.01)

    def test_reference_high(self):
        ec.setDualPointCalibration(0, 1.01, 0, 0)
        self.assertEqual(ec.getCalibrateHigh(), 1.01)

    def test_reading_low(self):
        ec.setDualPointCalibration(0, 0, 1.01, 0)
        self.assertEqual(ec.getCalibrateLowReading(), 1.01)

    def test_reading_high(self):
        ec.setDualPointCalibration(0, 0, 0, 1.01)
        self.assertEqual(ec.getCalibrateHighReading(), 1.01)

    def test_temperature(self):
        self.assertNotEqual(ec.measureTemp(), -127)

    def test_calibrate(self):
        ec.useDualPoint(False)
        ec.setK(1.0)
        ec.calibrateProbe(1.0, ec.tempCoefEC)
        self.assertAlmostEqual(first=ec.measureEC(), second=1.0, delta=0.5)

    def test_measure_ec(self):
        ec.useDualPoint(False)
        ec.setK(1.0)
        ec.calibrateProbe(1.0, ec.tempCoefEC)
        self.assertAlmostEqual(first=ec.measureEC(), second=1.0, delta=0.5)

    def test_reset(self):
        n = float('nan')
        ec.reset()
        self.assertTrue(math.isnan(ec.getCalibrateLow()))
        self.assertTrue(math.isnan(ec.getCalibrateHigh()))
        self.assertTrue(math.isnan(ec.getCalibrateLowReading()))
        self.assertTrue(math.isnan(ec.getCalibrateHighReading()))


if __name__ == '__main__':
    unittest.main()
    ec.reset()
    ec.useDualPoint(False)
