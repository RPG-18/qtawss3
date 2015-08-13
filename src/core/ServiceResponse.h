#pragma once

#include <memory>

#include <QtCore/QList>
#include <QtCore/QString>

class QIODevice;

namespace ASSS
{
    class ServiceResponsePrivate;
    class ServiceResponse
    {
    public:
        ServiceResponse();

        void parse(QIODevice* device);

        const QList<QString>& buckets() const;

    private:

        std::shared_ptr<ServiceResponsePrivate> m_impl;
    };

} /* namespace ASSS */
