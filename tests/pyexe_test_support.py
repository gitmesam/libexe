#!/usr/bin/env python
#
# Python-bindings support functions test script
#
# Copyright (C) 2011-2020, Joachim Metz <joachim.metz@gmail.com>
#
# Refer to AUTHORS for acknowledgements.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

import argparse
import os
import sys
import unittest

import pyexe


class SupportFunctionsTests(unittest.TestCase):
  """Tests the support functions."""

  def test_get_version(self):
    """Tests the get_version function."""
    version = pyexe.get_version()
    self.assertIsNotNone(version)

  def test_check_file_signature(self):
    """Tests the check_file_signature function."""
    if not unittest.source:
      raise unittest.SkipTest("missing source")

    result = pyexe.check_file_signature(unittest.source)
    self.assertTrue(result)

  def test_check_file_signature_file_object(self):
    """Tests the check_file_signature_file_object function."""
    if not unittest.source:
      raise unittest.SkipTest("missing source")

    with open(unittest.source, "rb") as file_object:
      result = pyexe.check_file_signature_file_object(file_object)
      self.assertTrue(result)

  def test_open(self):
    """Tests the open function."""
    if not unittest.source:
      raise unittest.SkipTest("missing source")

    exe_file = pyexe.open(unittest.source)
    self.assertIsNotNone(exe_file)

    exe_file.close()

    with self.assertRaises(TypeError):
      pyexe.open(None)

    with self.assertRaises(ValueError):
      pyexe.open(unittest.source, mode="w")

  def test_open_file_object(self):
    """Tests the open_file_object function."""
    if not unittest.source:
      raise unittest.SkipTest("missing source")

    if not os.path.isfile(unittest.source):
      raise unittest.SkipTest("source not a regular file")

    with open(unittest.source, "rb") as file_object:
      exe_file = pyexe.open_file_object(file_object)
      self.assertIsNotNone(exe_file)

      exe_file.close()

      with self.assertRaises(TypeError):
        pyexe.open_file_object(None)

      with self.assertRaises(ValueError):
        pyexe.open_file_object(file_object, mode="w")


if __name__ == "__main__":
  argument_parser = argparse.ArgumentParser()

  argument_parser.add_argument(
      "source", nargs="?", action="store", metavar="PATH",
      default=None, help="path of the source file.")

  options, unknown_options = argument_parser.parse_known_args()
  unknown_options.insert(0, sys.argv[0])

  setattr(unittest, "source", options.source)

  unittest.main(argv=unknown_options, verbosity=2)
