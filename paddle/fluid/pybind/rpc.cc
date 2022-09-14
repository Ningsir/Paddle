//   Copyright (c) 2022 PaddlePaddle Authors. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "paddle/fluid/pybind/rpc.h"
#include "paddle/fluid/distributed/rpc/future_wrapper.h"
#include "paddle/fluid/distributed/rpc/python_rpc_handler.h"
#include "paddle/fluid/distributed/rpc/rpc_agent.h"

namespace py = pybind11;
using paddle::distributed::FutureWrapper;
using paddle::distributed::PythonRpcHandler;
using paddle::distributed::RpcAgent;
using paddle::distributed::ServiceInfo;
namespace paddle {
namespace pybind {

void BindServiceInfo(py::module* m) {
  py::class_<ServiceInfo>(*m, "ServiceInfo")
      .def(py::init<std::string, uint32_t, std::string, uint32_t>())
      .def_readonly("name", &ServiceInfo::name_)
      .def_readonly("id", &ServiceInfo::id_)
      .def_readonly("ip", &ServiceInfo::ip_)
      .def_readonly("port", &ServiceInfo::port_);
}
void BindRpcAgent(py::module* m) {
  py::class_<RpcAgent, std::shared_ptr<RpcAgent>>(*m, "RpcAgent")
      .def(py::init<>([](std::string name, std::vector<ServiceInfo> infos) {
             // PythonRpcHandler::GetInstance();
             return std::shared_ptr<RpcAgent>(
                 new RpcAgent(std::move(name), std::move(infos)));
           }),
           py::arg("name"),
           py::arg("infos"))
      .def("start_server",
           &RpcAgent::StartServer,
           py::call_guard<py::gil_scoped_release>())
      .def("start_client",
           &RpcAgent::StartClient,
           py::call_guard<py::gil_scoped_release>())
      .def("stop", &RpcAgent::Stop, py::call_guard<py::gil_scoped_release>());
}
void BindFuture(py::module* m) {
  py::class_<FutureWrapper, std::shared_ptr<FutureWrapper>>(*m, "Future")
      // .def(py::init<std::future<std::string>>())
      .def(py::init<>())
      .def("wait",
           &FutureWrapper::wait,
           py::call_guard<py::gil_scoped_release>());
}
void SetAgentInstance(py::module* m) {
  m->def("set_agent_instance",
         &RpcAgent::SetAgentInstance,
         py::call_guard<py::gil_scoped_release>(),
         py::arg("agent"));
}
void InvokeRpc(py::module* m) {
  m->def(
      "invoke_rpc",
      [](const std::string& name, const std::string& py_func) {
        auto instance = RpcAgent::RpcAgentInstance();
        return std::make_shared<FutureWrapper>(
            instance->InvokeRpc(py_func, name));
      },
      py::call_guard<py::gil_scoped_release>(),
      py::arg("to"),
      py::arg("py_func"));
}
void StopServer(py::module* m) {
  m->def(
      "rpc_stop_server",
      []() {
        auto instance = RpcAgent::RpcAgentInstance();
        instance->Stop();
      },
      py::call_guard<py::gil_scoped_release>());
}
void GetServiceInfo(py::module* m) {
  m->def(
      "rpc_get_service_info",
      [](const std::string& name) {
        auto instance = RpcAgent::RpcAgentInstance();
        return instance->GetServiceInfo(name);
      },
      py::call_guard<py::gil_scoped_release>(),
      py::arg("name"));
}
void GetServiceInfoByRank(py::module* m) {
  m->def(
      "rpc_get_service_info_by_rank",
      [](uint32_t rank) {
        auto instance = RpcAgent::RpcAgentInstance();
        return instance->GetServiceInfoById(rank);
      },
      py::call_guard<py::gil_scoped_release>(),
      py::arg("rank"));
}
void GetCurrentServiceInfo(py::module* m) {
  m->def(
      "rpc_get_current_service_info",
      []() {
        auto instance = RpcAgent::RpcAgentInstance();
        return instance->GetCurrentServiceInfo();
      },
      py::call_guard<py::gil_scoped_release>());
}
void GetAllServiceInfos(py::module* m) {
  m->def(
      "rpc_get_all_service_infos",
      []() {
        auto instance = RpcAgent::RpcAgentInstance();
        return instance->GetAllServiceInfos();
      },
      py::call_guard<py::gil_scoped_release>());
}
void GetRank(py::module* m) {
  m->def(
      "rpc_get_rank",
      []() {
        auto instance = RpcAgent::RpcAgentInstance();
        return instance->Rank();
      },
      py::call_guard<py::gil_scoped_release>());
}
void GetWorldSize(py::module* m) {
  m->def(
      "rpc_get_world_size",
      []() {
        auto instance = RpcAgent::RpcAgentInstance();
        return instance->WorldSize();
      },
      py::call_guard<py::gil_scoped_release>());
}
void ClearPythonRpcHandler(py::module* m) {
  m->def("rpc_clear_python_rpc_handler", []() {
    auto instance = PythonRpcHandler::GetInstance();
    instance->Clear();
  });
}
}  // namespace pybind
}  // namespace paddle
