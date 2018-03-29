startup<-read.table("/home/wyatt/Documents/Arboreal/Source/DataNormal/create_tag_time.txt", header = TRUE)


startup<-startup[startup$time < 1,]
plot(log(time)~files, data = startup)
plot(log(time)~log(tags), data = startup)


fit<-lm(log(time)~log(tags), data = startup)
summary(fit)
plot(fit)

plot(fit$residuals~startup$files,ylab="Residuals", xlab="Files", main="Files Residual Plot")
plot(fit$residuals~startup$tags,ylab="Residuals", xlab="Tags", main="Tags Residual Plot")
