.. _sd-file-parameters:

Parameters
==========

.. index::
   pair: .sd file; parameters

Abstract parameters description is covered in
:ref:`relevant chapter in metadata module description <soclib_desc-parameter>`.

General syntax
--------------

Parameters are declared in the ``tmpl_parameters`` and
``instance_parameters`` statements. Their general syntax is

.. function:: parameter.ParamType(name, default = None)
   :noindex:

   :param name: Name of parameter. This is mandatory and must be
                unique for a module

   :param default: Default value for parameter

.. index::
   pair: parameter; Int

.. function:: parameter.Int(name, min = None, max = None, default = None)
   :noindex:

   :param min: Minimal valid value. No min if left blank.
   :param max: Maximal valid value. No max if left blank.

   An integer value. This can be used to contain an address or another
   numerical value. Integer size and signedness are irrelevant in
   metadata files: arbitrary precision is supported in
   soclib-cc.

   This can be used as template or instance parameter.

   Example for a simple module with integer parameters:

   .. code-block:: cpp

      namespace example1 {

      template<int bus_bitwidth>
      class ExampleModule
      {
          // ...
          ExampleModule(sc_module_name name, uint32_t base_address);
      };
   
      }

   Example::

     Module("caba:example_module",
            classname = "example1::ExampleModule",
            tmpl_parameters = [
               parameter.Int('bus_bitwidth'),
               ],
            instance_parameters = [
               parameter.Int('base_address'),
               ],
        )
      

.. index::
   pair: parameter; Bool

.. function:: parameter.Bool(name, default = None)
   :noindex:

   A boolean value. This can be used as template or instance
   parameter.

   Example for a simple module with boolean parameters:

   .. code-block:: cpp

      namespace example2 {

      template<bool support_atomic_transactions>
      class ExampleModule
      {
          // ...
          ExampleModule(sc_module_name name, bool is_master);
      };
   
      }

   Metadata::

     Module("caba:example2_module",
            classname = "example2::ExampleModule",
            tmpl_parameters = [
               parameter.Bool('support_atomic_transactions'),
               ],
            instance_parameters = [
               parameter.Bool('is_master'),
               ],
        )

.. index::
   pair: parameter; Float

.. function:: parameter.Float(name, default = None)
   :noindex:

   A floating-point value. This can be used as template or instance
   parameter.

.. index::
   pair: parameter; String

.. function:: parameter.String(name, default = None)
   :noindex:

   A character string constant.
   This can only be used as instance parameter.

   Example:

   .. code-block:: cpp

     #include <string>

     namespace foo {

     class MyModule
     {
         // ...
         MyModule(sc_module_name name, const std::string &datafile);
     };
   
     }

   Meadata::

     odule("caba:my_module",
            classname = "foo::MyModule",
            instance_parameters = [
               parameter.String('datafile'),
               ],
        )

.. index::
   pair: parameter; StringArray
 
.. function:: parameter.StringArray(name, default = None)
   :noindex:

   An array of character strings. This can be used to pass a
   ``std::vector<std::string>`` object to a constructor.
   This can only be used as instance parameter.

   Example:

   .. code-block:: cpp

     #include <vector>
     #include <string>

     namespace with_arrays {

     class MyModule
     {
         // ...
         MyModule(sc_module_name name, const std::vector<std::string> &datafiles);
     };
  
     }

   Metadata::

     Module("caba:my_array_module",
            classname = "with_arrays::MyModule",
            instance_parameters = [
               parameter.StringArray('datafiles'),
               ],
        )

.. index::
   pair: parameter; IntArray

.. function:: parameter.IntArray(name, default = None)
   :noindex:

   An array of integers. This can be used to pass a
   ``std::vector<int>`` object to a constructor.
   This can only be used as instance parameter.

   Example:

   .. code-block:: cpp

     #include <vector>

     namespace with_arrays {

     class MyModule2
     {
         // ...
         MyModule2(sc_module_name name, const std::vector<uint32_t> &addresses);
     };
  
     }

   Metadata::

     Module("caba:my_array_module2",
            classname = "with_arrays::MyModule2",
            instance_parameters = [
               parameter.IntArray('addresses'),
               ],
        )

.. index::
   pair: parameter; IntTab

.. function:: parameter.IntTab(name, default = None)
   :noindex:

   This is a soclib-specific datatype. This can be used to pass a
   ``soclib::common::IntTab()`` object to a constructor.
   This can only be used as instance parameter.

   Example:

   .. code-block:: cpp

     #include <int_tab.h>

     namespace soclib {
     namespace caba {

     class VciFoo
     {
         // ...
         MyModule2(sc_module_name name, const soclib::common::IntTab &index);
     };
  
     }
     }

   Metadata::

     Module("caba:vci_foo",
            classname = "soclib::caba::VciFoo",
            instance_parameters = [
               parameter.IntTab('index'),
               ],
        )

.. index::
   pair: parameter; Type

.. function:: parameter.Type(name, default = None)
   :noindex:

   A C++ type name.

   This can only be used as tempalte parameter.

   .. code-block:: cpp

     namespace mylib {

     template<typename T>
     class Fifo
     {
         // ...
         Fifo(sc_module_name name, const size_t depth);
     };
  
     }

   Metadata::

     Module("caba:mylib_fifo,
            classname = "mylib::Fifo",
            tmpl_parameters = [
               parameter.Type('T'),
               ],
            instance_parameters = [
               parameter.Int('depth'),
               ],
        )

.. index::
   pair: parameter; Module

.. function:: parameter.Module(name, typename = None, default = None)
   :noindex:

   :param typename: Name of module type in :ref:`md-index`.

   A module.

   * As template parameter, this is an object type:
  
     .. code-block:: cpp
  
       namespace mylib {
 
       class MyIss
       {
           // ...
       };

       template<typename iss_t>
       class MyCache
       {
           // ...
           MyCache(sc_module_name name);
       };
    
       }
 
     Metadata::
 
       Module("common:my_iss",
              classname = "mylib::MyIss",
          )

       Module("caba:my_cache",
              classname = "mylib::MyCache",
              tmpl_parameters = [
                 parameter.Module('iss_t'),
                 ],
          )
   
     Instanciation of ``"caba:my_cache"`` with ``iss_t =
     "common:my_iss"`` could make a netlister generate the following
     object declaration:

     .. code-block:: cpp

       mylib::MyCache<mylib::MyIss> obj("name");


   * As an instance parameter, this is an object reference:
  
     .. code-block:: cpp
  
       namespace mylib {
 
       class MyIss
       {
           // ...
       };

       class MyCache2
       {
           // ...
           MyCache(sc_module_name name, MyIss iss);
       };
    
       }
 
     Metadata::
 
       Module("common:my_iss",
              classname = "mylib::MyIss",
          )

       Module("caba:my_cache2",
              classname = "mylib::MyCache2",
              instance_parameters = [
                 parameter.Module('iss_t'),
                 ],
          )
   
     Instanciation of ``"caba:my_cache2"`` with ``iss_t =
     "common:my_iss"`` could make a netlister generate the following
     object declaration:

     .. code-block:: cpp

       mylib::MyIss iss("name");
       mylib::MyCache2 obj("name", iss);

Inheritance
-----------

Most important feature of parameter passing is inheritance of
parameters from one module to used ones. This is done through the
``parameter.Reference`` statement.

Example::

  Module("caba:my_base_module",
         classname = "MyBaseModule",
         tmpl_parameters = [
            parameter.Int('param_base'),
            ],
     )

  Module("caba:my_other_module",
         classname = "MyOtherModule",
         tmpl_parameters = [
            parameter.Int('param_other'),
            ],
         uses = [
            Uses('caba:my_base_module', param_base = parameter.Reference('param_other')),
            ],
     )

Instanciating ``caba:my_other_module`` will require setting
``param_other`` only, and its value will be propagated to
``param_base`` parameter of ``caba:my_base_module``.

Parameter references can also be used in port declarations::

  Port('caba:bit_in','p_irq', parameter.Reference('n_irq'))

Formulae
--------

When inheriting values, basic formulae can also be used on
``parameter.Reference``. Supported operators are ``+``, ``-``, ``%``,
``*``, ``/`` and ``**`` (power).

Example::

  Uses('caba:my_base_module', param_base = parameter.Reference('param_other') * 2),

Default values
--------------

If parameters are left unspecified by callers, it is possible to
provide default values. This is done through the ``default``
keyword. Example::

  instance_parameters = [
     parameter.Int("answer", default = 42),
     ],
